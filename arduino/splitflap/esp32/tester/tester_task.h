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
#include <TFT_eSPI.h>
#include <Wire.h>

#include "Adafruit_MCP23017.h"
#include "src/Adafruit_INA219.h"

#include "../core/splitflap_task.h"
#include "../core/task.h"

enum class TestState {
    BOOT,
    BOOT_POWER_TEST,

    READY,

    TEST_LOOPBACK,
    TEST_LEDS,
    TEST_CHECK_MOTOR_POWER,
    TEST_ENABLE_MOTOR_POWER,
    TEST_HOME_ALL,

    TEST_ABORTED,
    TEST_PASSED,
    TEST_FAILED,
};


class TesterTask : public Task<TesterTask> {
    friend class Task<TesterTask>; // Allow base Task to invoke protected run()

    public:
        TesterTask(SplitflapTask& splitflap_task, const uint8_t task_core);

    protected:
        void run();

    private:
        SplitflapTask& splitflap_task_;
        Adafruit_MCP23017 mcp_;
        Adafruit_INA219 ina219_;

        TestState state_ = TestState::BOOT;

        TFT_eSPI tft_ = TFT_eSPI();

        /** Full-size sprite used as a framebuffer */
        TFT_eSprite spr_ = TFT_eSprite(&tft_);

        void initializeIo();
        void initializeMcp();
        void initializeDisplay();

        void abort(String message);
        void pass();
        void fail(String message);

        void disablePower();
};
