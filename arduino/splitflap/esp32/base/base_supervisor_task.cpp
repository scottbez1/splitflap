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

    while (1) {
        digitalWrite(BASE_MASTER_EN_PIN, HIGH);

        for (uint8_t i = 0; i < NUM_POWER_CHANNELS; i++) {
            leds_[LED_CHANNEL_INDEX[i]].setRGB(40, 0, 70);
            FastLED.show();

            mcp_.digitalWrite(MCP_PIN_CHANNEL_EN[i], 1);
            delay(100);
            
            for (uint8_t j = 0; j < NUM_POWER_CHANNELS; j++) {
                float v = ina219_[j].getBusVoltage_V();
                float ma = ina219_[j].getCurrent_mA();
                Serial.printf("%dmV\t%dmA\n", (int)(v*1000), (int)ma);
            }
            Serial.println("-----------");

            delay(1000);
            leds_[LED_CHANNEL_INDEX[i]] = CRGB::Black;
            FastLED.show();
            mcp_.digitalWrite(MCP_PIN_CHANNEL_EN[i], 0);
        }
        
        digitalWrite(BASE_MASTER_EN_PIN, LOW);
        leds_[0] = CRGB::Blue;
        FastLED.show();
        leds_[0] = CRGB::Black;
        delay(5000);
    }
}
