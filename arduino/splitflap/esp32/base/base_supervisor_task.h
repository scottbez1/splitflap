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

#include "../splitflap/serial_task.h"
#include "../core/splitflap_task.h"
#include "../core/task.h"

#include "base_config.h"

class BaseSupervisorTask : public Task<BaseSupervisorTask> {
    friend class Task<BaseSupervisorTask>; // Allow base Task to invoke protected run()

    public:
        BaseSupervisorTask(SplitflapTask& splitflap_task, SerialTask& serial_task, const uint8_t task_core);

    protected:
        void run();

    private:
        static uint8_t getPowerChannelForModuleIndex(uint8_t module_index);

        SplitflapTask& splitflap_task_;
        SerialTask& serial_task_;

        CRGB leds_[NUM_LEDS];
        Adafruit_MCP23017 mcp_;
        Adafruit_INA219 ina219_[NUM_POWER_CHANNELS] = {
            Adafruit_INA219(0x40),
            Adafruit_INA219(0x41),
            Adafruit_INA219(0x42),
            Adafruit_INA219(0x43),
            Adafruit_INA219(0x44),
        };

        PB_SupervisorState last_sent_state_ = {};

        PB_SupervisorState_State state_ = PB_SupervisorState_State_UNKNOWN;
        PB_SupervisorState_FaultInfo fault_info_;

        SplitflapState splitflap_state_;
        float voltage_volts_[NUM_POWER_CHANNELS] = {};
        float current_amps_[NUM_POWER_CHANNELS] = {};
        bool channel_on_[NUM_POWER_CHANNELS] = {};
        bool channel_used_[NUM_POWER_CHANNELS] = {};

        bool channel_enabled_[NUM_POWER_CHANNELS] = {};
        uint32_t channel_current_out_of_range_count_[NUM_POWER_CHANNELS] = {};
        uint8_t channel_unexpected_power_count_[NUM_POWER_CHANNELS] = {};


        uint32_t startup_voltage_low_at_millis_ = 0;
        int8_t startup_enable_channel_ = -1;
        uint32_t startup_enable_channel_millis_ = 0;
        uint8_t startup_enable_channel_current_settle_count_ = 0;

        void runStateStartingVerifyPsuOff();
        void runStateStartingVerifyStartupVoltages();
        void runStateStartingEnableChannels();
        void runStateNormal();
        void runStateFault();

        void readPower();
        void updateSplitflapState();
        void fault(PB_SupervisorState_FaultInfo_FaultType type, const char* msg);
        void updateLeds();

        void setPowerChannel(uint8_t channel, bool on);
        void sendState();

};
