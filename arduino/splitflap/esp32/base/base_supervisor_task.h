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
#pragma once

#include <Arduino.h>
#include <FastLED.h>

#include "Adafruit_MCP23017.h"
#include "src/Adafruit_INA219.h"

#include "../core/logger.h"
#include "../core/splitflap_task.h"
#include "../core/task.h"

#include "base_config.h"

class BaseSupervisorTask : public Task<BaseSupervisorTask> {
    friend class Task<BaseSupervisorTask>; // Allow base Task to invoke protected run()

    public:
        BaseSupervisorTask(SplitflapTask& splitflapTask, Logger& logger, const uint8_t taskCore);

    protected:
        void run();

    private:
        SplitflapTask& splitflap_task_;
        Logger& logger_;

        CRGB leds_[NUM_LEDS];
        Adafruit_MCP23017 mcp_;

        bool channel_enabled_[NUM_POWER_CHANNELS];
        uint32_t channel_current_out_of_range_count_[NUM_POWER_CHANNELS];

        Adafruit_INA219 ina219_[NUM_POWER_CHANNELS] = {
            Adafruit_INA219(0x40),
            Adafruit_INA219(0x41),
            Adafruit_INA219(0x42),
            Adafruit_INA219(0x43),
            Adafruit_INA219(0x44),
        };

        static uint8_t getPowerChannelForModuleIndex(uint8_t module_index);

        void setPowerChannel(uint8_t channel, bool on);

};
