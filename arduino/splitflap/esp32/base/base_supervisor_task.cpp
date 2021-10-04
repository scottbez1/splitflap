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

/** Maps splitflap module index to the power channel index. */
uint8_t BaseSupervisorTask::getPowerChannelForModuleIndex(uint8_t module_index) {
    return module_index / 36;
}

BaseSupervisorTask::BaseSupervisorTask(SplitflapTask& splitflap_task, Logger& logger, const uint8_t task_core) :
        Task("BaseSupervisor", 4096, 1, task_core),
        splitflap_task_(splitflap_task),
        logger_(logger) {
}

void BaseSupervisorTask::run() {
    pinMode(BASE_MCP_NRESET_PIN, OUTPUT);
    pinMode(BASE_MASTER_EN_PIN, OUTPUT);

    pinMode(PIN_UP_BUTTON, INPUT);  // TDisplay has built-in pullup
    pinMode(PIN_DOWN_BUTTON, INPUT_PULLUP);

    digitalWrite(BASE_MCP_NRESET_PIN, HIGH);

    Wire.begin();
    Wire.setClock(400000);

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
    
    char buf[400];
    snprintf(buf, sizeof(buf), "My address: %d", addr);
    logger_.log(buf);

    float v;
    float ma;

    for (uint8_t i = 0; i < NUM_POWER_CHANNELS; i++) {
        setPowerChannel(i, false);
    }

    digitalWrite(BASE_MASTER_EN_PIN, LOW);

    logger_.log("Waiting for low motor input voltage for 5 seconds...");
    // Voltage must be low on all channels for at least 5 seconds before enabling PSU (to avoid fast switching in case of a WDT reset or other crash loop)
    uint32_t voltage_low_millis = millis();
    while (millis() - voltage_low_millis < 5000) {
        for (uint8_t i = 0; i < NUM_POWER_CHANNELS; i++) {
            v = ina219_[i].getBusVoltage_V();
            ma = ina219_[i].getCurrent_mA();
            if (v > 3 || ma > IDLE_CURRENT_MILLIAMPS) {
                voltage_low_millis = millis();
            }
        }
    }

    digitalWrite(BASE_MASTER_EN_PIN, HIGH);

    logger_.log("Waiting for voltage...");
    uint32_t voltage_found_at = 0;
    while (1) {
        for (uint8_t i = 0; i < NUM_POWER_CHANNELS; i++) {
            v = ina219_[i].getBusVoltage_V();
            ma = ina219_[i].getCurrent_mA();

            if (v > 11.5 && v < 13) {
                channel_enabled_[i] = true;
                if (voltage_found_at == 0) {
                    voltage_found_at = millis();
                }
            }
        }
        if (voltage_found_at != 0 && millis() - voltage_found_at > 100) {
            break;
        }
    }

    bool fault = false;

    for (uint8_t i = 0; i < NUM_POWER_CHANNELS; i++) {
        if (!channel_enabled_[i]) {
            continue;
        }

        snprintf(buf, sizeof(buf), "Enabling power channel %u", i);
        logger_.log(buf);

        setPowerChannel(i, true);
        uint8_t low_current = 0;
        bool current_settled = false;
        for (uint8_t j = 0; j < 250; j++) {
            ma = ina219_[i].getCurrent_mA();
            snprintf(buf, sizeof(buf), "%d mA", (int)ma);
            logger_.log(buf);
            if (ma < IDLE_CURRENT_MILLIAMPS) {
                low_current++;
                if (low_current >= 10) {
                    current_settled = true;
                    break;
                }
            } else {
                low_current = 0;
            }
            delay(1);
        }
        if (!current_settled) {
            snprintf(buf, sizeof(buf), "FAULT DURING STARTUP: Inrush current didn't settle on power channel %u!", i);
            logger_.log(buf);
            setPowerChannel(i, false);
            fault = true;
            break;
        }
    }

    if (!fault) {
        splitflap_task_.resetAll();
    }

    uint32_t last_report = 0;

    float avg_moving[NUM_POWER_CHANNELS] = {};
    float avg_homing[NUM_POWER_CHANNELS] = {};
    while (!fault) {
        uint32_t now = millis();

        SplitflapState splitflap_state = splitflap_task_.getState();

        if (!splitflap_state.loopbacks_ok) {
            logger_.log("Bad loopbacks; shutting down power!");
            fault = true;
            break;
        }

        uint8_t moving[NUM_POWER_CHANNELS] = {};
        uint8_t homing[NUM_POWER_CHANNELS] = {};
        for (uint8_t i = 0; i < NUM_MODULES; i++) {
            uint8_t power_channel = getPowerChannelForModuleIndex(i);
            if (splitflap_state.modules[i].moving) {
                if (splitflap_state.modules[i].state == State::LOOK_FOR_HOME) {
                    homing[power_channel]++;
                } else {
                    moving[power_channel]++;
                }
            }
        }

        const float alpha = 0.95;
        int32_t min_expected_channel_current[NUM_POWER_CHANNELS] = {};
        int32_t max_expected_channel_current[NUM_POWER_CHANNELS] = {};
        for (uint8_t i = 0; i < NUM_POWER_CHANNELS; i++) {
            // if (moving[i] > avg_moving[i]) {
            //     avg_moving[i] = moving[i];
            // } else {
            //     avg_moving[i] = alpha * avg_moving[i] + (1-alpha)* moving[i];
            // }
            // if (homing[i] > avg_homing[i]) {
            //     avg_homing[i] = homing[i];
            // } else {
            //     avg_homing[i] = alpha * avg_homing[i] + (1-alpha) * homing[i];
            // }
            min_expected_channel_current[i] = -5 + (moving[i] + homing[i]) * MIN_MODULE_CURRENT_MA;
            max_expected_channel_current[i] = IDLE_CURRENT_MILLIAMPS
                    + homing[i] * MAX_MODULE_CURRENT_HOMING_MA
                    + (moving[i] + 1) * MAX_MODULE_CURRENT_MOVING_MA;
        }

        float voltages[NUM_POWER_CHANNELS];
        float current_ma[NUM_POWER_CHANNELS];
        for (uint8_t i = 0; i < NUM_POWER_CHANNELS; i++) {
            voltages[i] = ina219_[i].getBusVoltage_V();
            current_ma[i] = ina219_[i].getCurrent_mA();

            if (channel_enabled_[i]) {
                if (voltages[i] < 11 || voltages[i] > 13 || current_ma[i] > MAX_CHANNEL_CURRENT_MA) {
                    snprintf(buf, sizeof(buf), "BAD POWER on power channel %u! %dmV\t%dmA", i, (int)(voltages[i]*1000), (int)current_ma[i]);
                    logger_.log(buf);
                    fault = true;
                    break;
                }

                // FIXME
                // if (current_ma[i] < min_expected_channel_current[i] || current_ma[i] > max_expected_channel_current[i]) {
                if (current_ma[i] > max_expected_channel_current[i]) {
                    channel_current_out_of_range_count_[i]++;
                    if (channel_current_out_of_range_count_[i] >= CONSECUTIVE_CURRENT_OUT_OF_RANGE_THRESHOLD) {
                        snprintf(buf, sizeof(buf), "Power outside expected current on power channel %u!\n  Expected: %d - %dmA\n  Actual: %dmA", i, min_expected_channel_current[i], max_expected_channel_current[i], (int)current_ma[i]);
                        logger_.log(buf);
                        fault = true;
                        break;
                    }
                } else {
                    channel_current_out_of_range_count_[i] = 0;
                }

                leds_[LED_CHANNEL_INDEX[i]].setRGB(0, 30, ((now / 128) % 2 == 0) ?
                        (current_ma[i] > 25 ? 20 + current_ma[i]*100/MAX_CHANNEL_CURRENT_MA : 0) : 
                        0); 
            } else {
                if (voltages[i] > 5 || current_ma[i] > IDLE_CURRENT_MILLIAMPS) {
                    snprintf(buf, sizeof(buf), "BAD POWER on disabled power channel %u! %dmV\t%dmA", i, (int)(voltages[i]*1000), (int)current_ma[i]);
                    logger_.log(buf);
                    fault = true;
                    break;
                }
            }
        }
        FastLED.show();

        if (now - last_report > 1000) {
            last_report = now;
            float current_sum_ma = 0;
            for (uint8_t i = 0; i < NUM_POWER_CHANNELS; i++) {
                snprintf(buf, sizeof(buf), "%dmV\t%dmA\t\t[%d]", (int)(voltages[i]*1000), (int)current_ma[i], (int)max_expected_channel_current[i]);
                logger_.log(buf);
                current_sum_ma += current_ma[i];
            }
            snprintf(buf, sizeof(buf), "Total current: %d", (int)current_sum_ma);
            logger_.log(buf);
            logger_.log("----------");
        }
        delay(1);
    }

    // Shutdown
    for (uint8_t i = 0; i < NUM_POWER_CHANNELS; i++) {
        setPowerChannel(i, false);
    }
    digitalWrite(BASE_MASTER_EN_PIN, LOW);
    // TODO: shut down splitflap task?
    while (1) {
        delay(1000);
    }
}

void BaseSupervisorTask::setPowerChannel(uint8_t channel, bool on) {
    mcp_.digitalWrite(MCP_PIN_CHANNEL_EN[channel], on);
    leds_[LED_CHANNEL_INDEX[channel]] = on ? CRGB::Green : CRGB::Black;
    FastLED.show();
}
