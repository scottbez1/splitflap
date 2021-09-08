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

BaseSupervisorTask::BaseSupervisorTask(SplitflapTask& splitflap_task, const uint8_t task_core) : Task{"BaseSupervisor", 2048, 1, task_core}, splitflap_task_{splitflap_task} {}

void BaseSupervisorTask::run() {
    pinMode(BASE_MCP_NRESET_PIN, OUTPUT);
    pinMode(BASE_MASTER_EN_PIN, OUTPUT);

    digitalWrite(BASE_MCP_NRESET_PIN, HIGH);

    Wire.begin();
    Wire.setClock(400000);

    mcp_.begin(BASE_MCP_ADDRESS, &Wire);

    for (uint8_t i = 0; i < NUM_POWER_CHANNELS; i++) {
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
    
    Serial.printf("My address: %d\n", addr);

    float v;
    float ma;

    for (uint8_t i = 0; i < NUM_POWER_CHANNELS; i++) {
        setPowerChannel(i, false);
    }
    digitalWrite(BASE_MASTER_EN_PIN, LOW);

    for (uint8_t i = 0; i < NUM_POWER_CHANNELS; i++) {
        v = ina219_[i].getBusVoltage_V();
        ma = ina219_[i].getCurrent_mA();
        if (v > 3 || ma > IDLE_CURRENT_MILLIAMPS) {
            while (1) {
                Serial.printf("FAULT DURING STARTUP. Unexpected power on channel %u! %dmV\t%dmA\n", i, (int)(v*1000), (int)ma);
                delay(1000);
            }
        }
    }

    digitalWrite(BASE_MASTER_EN_PIN, HIGH);

    Serial.println("Waiting for voltage...");
    while (1) {
        v = ina219_[0].getBusVoltage_V();
        ma = ina219_[0].getCurrent_mA();

        if (v > 11.5 && v < 13) {
            break;
        }
    }

    bool fault = false;

    for (uint8_t i = 0; i < NUM_POWER_CHANNELS; i++) {
        Serial.printf("Enabling power channel %u\n", i);

        setPowerChannel(i, true);
        uint8_t low_current = 0;
        bool current_settled = false;
        for (uint8_t j = 0; j < 250; j++) {
            ma = ina219_[i].getCurrent_mA();
            Serial.printf("%d mA\n", (int)ma);
            if (ma < 20) {
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
            Serial.printf("FAULT DURING STARTUP: Inrush current didn't settle on power channel %u!\n", i);
            setPowerChannel(i, false);
            fault = true;
            break;
        }
    }

    if (!fault) {
        // splitflap_task_.resetAll();

        Command c = {};
        for (uint8_t i = 0; i < 82; i++) {
            c.data[i] = QCMD_RESET_AND_HOME;
        }
        splitflap_task_.postRawCommandToBack(c);

        uint32_t last_report = 0;
        while (!fault) {
            uint32_t now = millis();
            float voltages[NUM_POWER_CHANNELS];
            float current_ma[NUM_POWER_CHANNELS];
            for (uint8_t i = 0; i < NUM_POWER_CHANNELS; i++) {
                voltages[i] = ina219_[i].getBusVoltage_V();
                current_ma[i] = ina219_[i].getCurrent_mA();

                // TODO: calculate expected load based on splitflap moving status
                if ((voltages[i] > 5 && voltages[i] < 11) || voltages[i] > 13 || current_ma[i] > 9000) {
                    Serial.printf("BAD POWER on power channel %u! %dmV\t%dmA\n", i, (int)(voltages[i]*1000), (int)current_ma[i]);
                    fault = true;
                    break;
                }

                leds_[LED_CHANNEL_INDEX[i]].setRGB(0, 30, ((now / 128) % 2 == 0) ? (current_ma[i] > 25 ? 20 + current_ma[i]*100/9000 : 0) : 0);
            }
            FastLED.show();

            if (now - last_report > 500) {
                last_report = now;
                float current_sum_ma = 0;
                for (uint8_t i = 0; i < NUM_POWER_CHANNELS; i++) {
                    Serial.printf("%dmV\t%dmA\n", (int)(voltages[i]*1000), (int)current_ma[i]);
                    current_sum_ma += current_ma[i];
                }
                Serial.printf("Total current: %d\n", (int)current_sum_ma);
                Serial.println("----------");
            }
        }
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
