/*
   Copyright 2021 Scott Bezek and the splitflap contributors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include <FastLED.h>

#include "base_supervisor_task.h"

#define PIN_UP_BUTTON           35
#define PIN_DOWN_BUTTON         0


/**
 * MODIFY THIS to configure which modules are connected to which power channels!
 */
uint8_t BaseSupervisorTask::getPowerChannelForModuleIndex(uint8_t module_index) {
    return module_index / 36;
}

/** Maps power channel index (0-4) to MCP GPIO pin. */
static const uint8_t MCP_PIN_CHANNEL_EN[NUM_POWER_CHANNELS] = {
    5,
    4,
    3,
    2,
    1
};

/** Maps power channel index (0-4) to an LED index in the neopixel chain. */
static const uint8_t LED_CHANNEL_INDEX[NUM_POWER_CHANNELS] = {
    5,
    4,
    3,
    2,
    1
};

BaseSupervisorTask::BaseSupervisorTask(SplitflapTask& splitflap_task, SerialTask& serial_task, const uint8_t task_core) :
        Task("BaseSupervisor", 8192, 1, task_core),
        splitflap_task_(splitflap_task),
        serial_task_(serial_task) {
}

void BaseSupervisorTask::run() {
    pinMode(BASE_MCP_NRESET_PIN, OUTPUT);
    pinMode(BASE_MASTER_EN_PIN, OUTPUT);

    pinMode(PIN_UP_BUTTON, INPUT);  // TDisplay has built-in pullup
    pinMode(PIN_DOWN_BUTTON, INPUT_PULLUP);

    digitalWrite(BASE_MCP_NRESET_PIN, HIGH);

    Wire.begin();

    mcp_.begin(BASE_MCP_ADDRESS, &Wire);

    for (uint8_t i = 0; i < NUM_POWER_CHANNELS; i++) {
        channel_enabled_[i] = false;
        channel_current_out_of_range_count_[i] = 0;

        ina219_[i].begin();
        ina219_[i].setCalibrationSplitflap();

        mcp_.pinMode(MCP_PIN_CHANNEL_EN[i], OUTPUT);
    }

    mcp_.pinMode(MCP_PIN_ADDR_1, INPUT);
    mcp_.pullUp(MCP_PIN_ADDR_1, 1);
    mcp_.pinMode(MCP_PIN_ADDR_2, INPUT);
    mcp_.pullUp(MCP_PIN_ADDR_2, 1);
    mcp_.pinMode(MCP_PIN_ADDR_4, INPUT);
    mcp_.pullUp(MCP_PIN_ADDR_4, 1);
    mcp_.pinMode(MCP_PIN_ADDR_8, INPUT);
    mcp_.pullUp(MCP_PIN_ADDR_8, 1);

    FastLED.addLeds<NEOPIXEL, BASE_NEOPIXEL_PIN>(leds_, NUM_LEDS);

    bool addr1 = !mcp_.digitalRead(MCP_PIN_ADDR_1);
    bool addr2 = !mcp_.digitalRead(MCP_PIN_ADDR_2);
    bool addr4 = !mcp_.digitalRead(MCP_PIN_ADDR_4);
    bool addr8 = !mcp_.digitalRead(MCP_PIN_ADDR_8);
    uint8_t addr = (addr8 << 3) |
                    (addr4 << 2) |
                    (addr2 << 1) |
                    (addr1);
    
    char buf[255];
    snprintf(buf, sizeof(buf), "My address: %d", addr);
    serial_task_.log(buf);

    for (uint8_t i = 0; i < NUM_POWER_CHANNELS; i++) {
        setPowerChannel(i, false);
    }

    digitalWrite(BASE_MASTER_EN_PIN, LOW);

    state_ = PB_SupervisorState_State_STARTING_VERIFY_PSU_OFF;
    fault_info_.type = PB_SupervisorState_FaultInfo_FaultType_NONE;
    snprintf(fault_info_.msg, sizeof(fault_info_.msg), "");
    sendState();

    // Determine which channels are actually used, based on the power channel mapping function.
    // (This could technically be determined statically since the mapping is static, but it's
    // easier to just compute it at runtime)
    for (uint8_t i = 0; i < NUM_MODULES; i++) {
        channel_used_[getPowerChannelForModuleIndex(i)] = true;
    }

    while (1) {
        readPower();
        updateSplitflapState();
        switch (state_) {
            case PB_SupervisorState_State_STARTING_VERIFY_PSU_OFF:
                runStateStartingVerifyPsuOff();
                break;
            case PB_SupervisorState_State_STARTING_VERIFY_VOLTAGES:
                runStateStartingVerifyStartupVoltages();
                break;
            case PB_SupervisorState_State_STARTING_ENABLE_CHANNELS:
                runStateStartingEnableChannels();
                break;
            case PB_SupervisorState_State_NORMAL:
                runStateNormal();
                break;
            case PB_SupervisorState_State_FAULT:
                runStateFault();
                break;
            default:
                assert(false);
        }
        sendState();
        updateLeds();
        delay(1);
    }

    while (1) {
        delay(1);
    }
}

void BaseSupervisorTask::runStateStartingVerifyPsuOff() {
    // Voltage must be low on all channels for at least 5 seconds before enabling PSU (to avoid fast switching in case of a WDT reset or other crash loop)
    for (uint8_t i = 0; i < NUM_POWER_CHANNELS; i++) {
        if (voltage_volts_[i] > MAX_DISABLED_VOLTAGE || current_amps_[i] * 1000 > IDLE_CURRENT_MILLIAMPS) {
            startup_voltage_low_at_millis_ = millis();
        }
    }

    if (millis() - startup_voltage_low_at_millis_ > 5000) {
        serial_task_.log("Startup voltages low = ok. Enabling PSU...");
        digitalWrite(BASE_MASTER_EN_PIN, HIGH);
        state_ = PB_SupervisorState_State_STARTING_VERIFY_VOLTAGES;
    }
}

void BaseSupervisorTask::runStateStartingVerifyStartupVoltages() {
    bool all_ok = true;
    for (uint8_t i = 0; i < NUM_POWER_CHANNELS; i++) {
        if (channel_used_[i]) {
            all_ok &= voltage_volts_[i] >= MIN_RUN_VOLTAGE && voltage_volts_[i] < ABSOLUTE_MAX_VOLTAGE;
        } else {
            all_ok &= voltage_volts_[i] < MAX_DISABLED_VOLTAGE;
        }
    }
    if (all_ok) {
        startup_enable_channel_ = -1;
        state_ = PB_SupervisorState_State_STARTING_ENABLE_CHANNELS;
    }
}

void BaseSupervisorTask::runStateStartingEnableChannels() {
    if (startup_enable_channel_ == -1) {
        startup_enable_channel_ = 0;
        startup_enable_channel_millis_ = millis();
        startup_enable_channel_current_settle_count_ = 0;
        setPowerChannel(0, true);
        return;
    }

    if (millis() - startup_enable_channel_millis_ > 500) {
        char msg[255];
        snprintf(msg, sizeof(msg), "Inrush current didn't settle on power channel %u!", startup_enable_channel_);
        fault(PB_SupervisorState_FaultInfo_FaultType_INRUSH_CURRENT_NOT_SETTLED, msg);
        return;
    }

    if (current_amps_[startup_enable_channel_] * 1000 < IDLE_CURRENT_MILLIAMPS) {
        startup_enable_channel_current_settle_count_++;
        if (startup_enable_channel_current_settle_count_ >= 10) {
            char msg[255];
            snprintf(msg, sizeof(msg), "Current settled on channel %u", startup_enable_channel_);
            serial_task_.log(msg);

            // Determine the next channel to enable
            do {
                startup_enable_channel_++;
            } while (startup_enable_channel_ < NUM_POWER_CHANNELS && !channel_used_[startup_enable_channel_]);

            if (startup_enable_channel_ >= NUM_POWER_CHANNELS) {
                serial_task_.log("All channels enabled, startup complete!");
                state_ = PB_SupervisorState_State_NORMAL;
                splitflap_task_.resetAll();
                return;
            }

            startup_enable_channel_millis_ = millis();
            startup_enable_channel_current_settle_count_ = 0;
            setPowerChannel(startup_enable_channel_, true);
        }
    } else {
        startup_enable_channel_current_settle_count_ = 0;
    }
}

void BaseSupervisorTask::runStateNormal() {
    if (!splitflap_state_.loopbacks_ok) {
        fault(PB_SupervisorState_FaultInfo_FaultType_SPLITFLAP_SHUTDOWN, "Bad loopbacks; shutting down power!");
        return;
    }
    char msg[255];

    uint8_t moving[NUM_POWER_CHANNELS] = {};
    uint8_t homing[NUM_POWER_CHANNELS] = {};
    for (uint8_t i = 0; i < NUM_MODULES; i++) {
        uint8_t power_channel = getPowerChannelForModuleIndex(i);
        if (splitflap_state_.modules[i].moving) {
            if (splitflap_state_.modules[i].state == State::LOOK_FOR_HOME) {
                homing[power_channel]++;
            } else {
                moving[power_channel]++;
            }
        }
    }

    for (uint8_t i = 0; i < NUM_POWER_CHANNELS; i++) {
        if (channel_used_[i]) {
            if (voltage_volts_[i] < MIN_RUN_VOLTAGE || voltage_volts_[i] > ABSOLUTE_MAX_VOLTAGE || current_amps_[i] * 1000 > ABSOLUTE_MAX_CHANNEL_CURRENT_MA) {
                snprintf(msg, sizeof(msg), "Bad power on channel %u! %.2fV   %.3fA", i, voltage_volts_[i], current_amps_[i]);
                fault(PB_SupervisorState_FaultInfo_FaultType_OUT_OF_RANGE, msg);
                return;
            }

            float min_expected_channel_current_ma = -5 + (moving[i] + homing[i]) * MIN_MODULE_CURRENT_MA;
            float max_expected_channel_current_ma = IDLE_CURRENT_MILLIAMPS
                    + homing[i] * MAX_MODULE_CURRENT_HOMING_MA
                    + (moving[i] > 0 ? (moving[i] + 2) : 1) * MAX_MODULE_CURRENT_MOVING_MA;

            if (current_amps_[i] * 1000 > max_expected_channel_current_ma) {
                channel_current_out_of_range_count_[i]++;
                if (channel_current_out_of_range_count_[i] >= CONSECUTIVE_CURRENT_OUT_OF_RANGE_THRESHOLD) {
                    snprintf(msg, sizeof(msg), "Over current on channel %u!\n  Expected up to %.3fA\n  Actual: %.3fA", i, max_expected_channel_current_ma/1000, current_amps_[i]);
                    fault(PB_SupervisorState_FaultInfo_FaultType_OVER_CURRENT, msg);
                    return;
                }
            } else {
                channel_current_out_of_range_count_[i] = 0;
            }

            leds_[LED_CHANNEL_INDEX[i]].setRGB(
                0,
                30,
                (millis() / 128) % 2 == 0 && current_amps_[i] * 1000 > IDLE_CURRENT_MILLIAMPS ? 
                        20 + current_amps_[i]*1000 / ABSOLUTE_MAX_CHANNEL_CURRENT_MA * 100
                        :
                        0
            );
        } else {
            leds_[LED_CHANNEL_INDEX[i]].setRGB(0, 0, 0);
            if (voltage_volts_[i] > 5 || current_amps_[i] * 1000 > IDLE_CURRENT_MILLIAMPS) {
                channel_unexpected_power_count_[i] ++;
                if (channel_unexpected_power_count_[i] >= CONSECUTIVE_UNEXPECTED_POWER_THRESHOLD) {
                    snprintf(msg, sizeof(msg), "Unexpected power on disabled power channel %u! %.2fV\t%.3fA", i, voltage_volts_[i], current_amps_[i]);
                    fault(PB_SupervisorState_FaultInfo_FaultType_UNEXPECTED_POWER, msg);
                    return;
                }
            } else {
                channel_unexpected_power_count_[i] = 0;
            }
        }
    }
}

void BaseSupervisorTask::runStateFault() {
    // Nothing to do, and no way to exit (except a reset)
}


void BaseSupervisorTask::setPowerChannel(uint8_t channel, bool on) {
    channel_on_[channel] = on;
    mcp_.digitalWrite(MCP_PIN_CHANNEL_EN[channel], on);
    leds_[LED_CHANNEL_INDEX[channel]] = on ? CRGB::Green : CRGB::Black;
    FastLED.show();
}

void BaseSupervisorTask::sendState() {
    bool should_send = false;

    PB_SupervisorState state = {};
    state.state = state_;
    should_send |= state.state != last_sent_state_.state;

    state.uptime_millis = millis();
    should_send |= state.uptime_millis - last_sent_state_.uptime_millis > 2000; // Send periodically

    state.power_channels_count = NUM_POWER_CHANNELS;
    for (uint8_t i = 0; i < NUM_POWER_CHANNELS; i++) {
        state.power_channels[i].voltage_volts = voltage_volts_[i];
        should_send |= fabs(state.power_channels[i].voltage_volts - last_sent_state_.power_channels[i].voltage_volts) > 0.2f;

        state.power_channels[i].current_amps = current_amps_[i];
        should_send |= fabs(state.power_channels[i].current_amps - last_sent_state_.power_channels[i].current_amps) > 0.1f;

        state.power_channels[i].on = channel_on_[i];
        should_send |= state.power_channels[i].on != last_sent_state_.power_channels[i].on;
    }

    state.fault_info = fault_info_;
    state.has_fault_info = true;

    // Rate limit transmissions
    if (should_send && state.uptime_millis - last_sent_state_.uptime_millis > 250) {
        serial_task_.sendSupervisorState(state);
        last_sent_state_ = state;
    }
}

void BaseSupervisorTask::readPower() {
    for (uint8_t i = 0; i < NUM_POWER_CHANNELS; i++) {
        voltage_volts_[i] = ina219_[i].getBusVoltage_V();
        current_amps_[i] = ina219_[i].getCurrent_mA() / 1000.;

        // Check for absolute max violations
        if (state_ != PB_SupervisorState_State_FAULT && (
            voltage_volts_[i] > ABSOLUTE_MAX_VOLTAGE || current_amps_[i] * 1000 > ABSOLUTE_MAX_CHANNEL_CURRENT_MA
        )) {
            char msg[255];
            snprintf(msg, sizeof(msg), "Absolute max exceeded on channel %u. %.2fV %.3fA", i, voltage_volts_[i], current_amps_[i]);
            fault(PB_SupervisorState_FaultInfo_FaultType_OUT_OF_RANGE, msg);
            return;
        }
    }
}

void BaseSupervisorTask::updateSplitflapState() {
    splitflap_state_ = splitflap_task_.getState();
}

void BaseSupervisorTask::fault(PB_SupervisorState_FaultInfo_FaultType type, const char* msg) {
    for (uint8_t i = 0; i < NUM_POWER_CHANNELS; i++) {
        setPowerChannel(i, false);
    }
    digitalWrite(BASE_MASTER_EN_PIN, LOW);

    state_ = PB_SupervisorState_State_FAULT;
    strncpy(fault_info_.msg, msg, sizeof(fault_info_.msg));
    fault_info_.type = type;
    fault_info_.ts_millis = millis();
    sendState();
    splitflap_task_.disableAll();
    serial_task_.log(msg);
}

void BaseSupervisorTask::updateLeds() {
    CRGB main_status;
    switch (state_) {
        case PB_SupervisorState_State_STARTING_VERIFY_PSU_OFF:
            main_status = CRGB::DarkViolet;
            break;
        case PB_SupervisorState_State_STARTING_VERIFY_VOLTAGES:
            main_status = CRGB::DarkGray;
            break;
        case PB_SupervisorState_State_STARTING_ENABLE_CHANNELS:
            main_status = CRGB::DeepSkyBlue;
            break;
        case PB_SupervisorState_State_NORMAL:
            main_status = CRGB::Green;
            break;
        case PB_SupervisorState_State_FAULT:
            main_status = ((millis() / 250) % 2) ? CRGB::Red : CRGB::Black;
            break;
        default:
            assert(false);
    }
    leds_[0] = main_status;

    FastLED.show();
}