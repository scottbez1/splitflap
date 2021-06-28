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

#include "reporter.h"
#include "result.h"
#include "../core/splitflap_task.h"
#include "../core/task.h"

// enum class TestState : uint16_t {
//     BOOT                        = 0,
//     BOOT_POWER_TEST             = 1,

//     READY                       = 100,

//     TEST_LOOPBACK               = 200,
//     TEST_LEDS                   = 210,
//     TEST_CHECK_MOTOR_POWER      = 220,
//     TEST_ENABLE_MOTOR_POWER     = 230,
//     TEST_HOME_ALL               = 240,

//     TEST_MIN                    = TEST_LOOPBACK,
//     TEST_MAX                    = TEST_HOME_ALL,

//     RESULT_ABORTED                = 900,
//     RESULT_PASSED                 = 901,
//     RESULT_FAILED                 = 902,
// };


class Status {
    public:
        enum class Code : uint8_t {
            OK = 0,
            FATAL_ERROR = 1,
        };

        const Code code_;
        const String message_;

        bool isOk() {
            return code_ == Code::OK;
        }

        static Status fatal(String message) {
            return Status(Code::FATAL_ERROR, message);
        }

        static Status ok() {
            return Status(Code::OK, "");
        }

    private:
        Status(const Code code, const String message) : code_(code), message_(message) {};
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

        TFT_eSPI tft_ = TFT_eSPI();

        const uint32_t COLOR_ACTION = tft_.color565(20, 20, 200);

        /** Full-size sprite used as a framebuffer */
        TFT_eSprite spr_ = TFT_eSprite(&tft_);

        Reporter reporter_;

        // General state
        float voltage_;
        float current_;
        bool clamped_;

        String message_;

        void initializeIo();
        void initializeMcp();
        void initializeDisplay();

        void disableHardware();
        void drawSimpleText(uint32_t background, uint32_t foreground, String title, String details);

        Status doMaintenance();
        Result doTestMaintenance();
        Status runStartupSelfTest();

        Result runTestSuite();
        Status runTestSuitesForever();

        Status waitForBoardInserted();
        Status waitForBoardRemoved();
        String readSerial();
        Result testLeds();



};
