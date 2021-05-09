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
#include "config.h"
#include "src/splitflap_module.h"
#include "src/spi_io_config.h"

#include "tester_task.h"

#define PIN_MOTOR_POWER 13
#define PIN_MOTOR_TEST_FEED 27
#define PIN_MCP_NRESET 26
#define PIN_BUTTON_BOTTOM 0

#define MCP_PIN_BUZZER 11
#define MCP_PIN_CLAMP 7

TesterTask::TesterTask(SplitflapTask& splitflap_task, const uint8_t task_core) : Task{"Tester", 8192, 1, task_core}, splitflap_task_{splitflap_task_} {
}

void TesterTask::run() {
    initializeIo();
    initializeMcp();
    ina219_.setCalibrationSplitflap();
    initializeDisplay();

    bool button_state_bottom = !digitalRead(PIN_BUTTON_BOTTOM);
    TestState last_state = TestState::BOOT;
    uint32_t last_state_change = millis();
    uint32_t time_clamped = 0;

    while (1) {
        uint32_t time_in_state = millis() - last_state_change;
        spr_.fillSprite(TFT_BLACK);

        float voltage = ina219_.getBusVoltage_V();
        float current = ina219_.getCurrent_mA();
        if (voltage > 13) {
            abort("Over-voltage: " + String(voltage));
        } else if (current > 2000) {
            abort("Over-current: " + String(current));
        }

        // TODO: check 3v3_fused status

        bool clamped = !mcp_.digitalRead(MCP_PIN_CLAMP);

        // TODO: check for unclamped board during any TEST_ step

        bool new_button_state_bottom = !digitalRead(PIN_BUTTON_BOTTOM);
        // TODO: debounce
        bool button_bottom_pressed = new_button_state_bottom && !button_state_bottom;
        button_state_bottom = new_button_state_bottom;

        uint32_t bg = 0;
        switch (state_) {
            case TestState::BOOT:
                if (clamped) {
                    bg = spr_.color565(100, 0, 0);
                    spr_.fillSprite(bg);
                    spr_.setTextColor(TFT_WHITE, bg);
                    spr_.setTextSize(2);
                    spr_.setCursor(10, 10);
                    spr_.printf("Un-clamp test board");
                } else if (button_bottom_pressed) {
                    state_ = TestState::BOOT_POWER_TEST;
                    digitalWrite(PIN_MOTOR_POWER, HIGH);
                } else {
                    bg = spr_.color565(20, 20, 200);
                    spr_.fillSprite(bg);
                    spr_.setTextColor(TFT_WHITE, bg);
                    spr_.setTextSize(2);
                    spr_.setCursor(10, 10);
                    spr_.printf("Remove test board, then press GO");
                }
                break;
            case TestState::BOOT_POWER_TEST:
                spr_.setTextColor(TFT_WHITE, TFT_BLACK);
                spr_.setTextSize(2);
                spr_.setCursor(10, 10);
                spr_.printf("Checking power...");
                if (voltage > 11.8) {
                    digitalWrite(PIN_MOTOR_POWER, LOW);
                    state_ = TestState::READY;
                } else if (time_in_state > 200 && current > 10) {
                    abort("Over-current: " + String(current));
                } else if (time_in_state > 1000) {
                    abort("Timeout waiting for power");
                }
                break;
            case TestState::READY:
                bg = spr_.color565(20, 20, 200);
                spr_.fillSprite(bg);
                spr_.setTextColor(TFT_WHITE, bg);
                spr_.setTextSize(2);
                spr_.setCursor(10, 10);
                spr_.printf("Insert board");
                if (!clamped) {
                    time_clamped = 0;
                } else {
                    if (time_clamped == 0) {
                        time_clamped = millis();
                    } else if (millis() - time_clamped > 1500) {
                        state_ = TestState::TEST_LOOPBACK;
                    }
                }
                break;
            case TestState::TEST_LOOPBACK: {
                bool passed = true;
                for (uint8_t i = 0; i < NUM_LOOPBACKS; i++) {
                    if (!chainlink_test_loopback(i)) {
                        fail("Failed loopback " + String(i));
                        passed = false;
                        break;
                    }
                }
                if (!chainlink_test_startup_loopback()) {
                    fail("Failed startup loopback!");
                    passed = false;
                }

                if (passed) {
                    state_ = TestState::TEST_LEDS;
                }
                break;
            }
            case TestState::TEST_LEDS:
                // TODO!

                state_ = TestState::TEST_CHECK_MOTOR_POWER;
                digitalWrite(PIN_MOTOR_TEST_FEED, HIGH);
                break;
            case TestState::TEST_CHECK_MOTOR_POWER:
                spr_.setTextColor(TFT_WHITE, TFT_BLACK);
                spr_.setTextSize(2);
                spr_.setCursor(10, 10);
                spr_.printf("Preparing power...");
                if (voltage > 3.3) {
                    fail("Over-voltage during test feed: " + String(voltage));
                } else if (time_in_state > 200 && current > 10) {
                    fail("Over-current during test feed: " + String(current));
                } else if (time_in_state > 200 && voltage > 2.5) {
                    state_ = TestState::TEST_ENABLE_MOTOR_POWER;
                    digitalWrite(PIN_MOTOR_TEST_FEED, LOW);
                    digitalWrite(PIN_MOTOR_POWER, HIGH);
                } else if (time_in_state > 1000) {
                    fail("Timeout waiting for power");
                }
                break;
            case TestState::TEST_ENABLE_MOTOR_POWER:
                spr_.setTextColor(TFT_WHITE, TFT_BLACK);
                spr_.setTextSize(2);
                spr_.setCursor(10, 10);
                spr_.printf("Checking power...");
                if (voltage > 11.8) {
                    state_ = TestState::TEST_HOME_ALL;
                    splitflap_task_.resetAll();
                } else if (time_in_state > 200 && current > 10) {
                    fail("Over-current: " + String(current));
                } else if (time_in_state > 1000) {
                    fail("Timeout waiting for power");
                }
                break;
            
            case TestState::TEST_HOME_ALL:
                spr_.setTextColor(TFT_WHITE, TFT_BLACK);
                spr_.setTextSize(2);
                spr_.setCursor(10, 10);
                spr_.printf("Home calibration...");

                // TODO: real test!
                // TODO: figure out splitflap updates without delays...
                if (time_in_state > 10000) {
                    pass();
                }
                break;

            case TestState::TEST_ABORTED:
                bg = spr_.color565(200, 25, 0);
                spr_.fillSprite(bg);
                spr_.setTextColor(TFT_WHITE, bg);
                spr_.setTextSize(3);
                spr_.setCursor(10, 10);
                spr_.printf("Aborted!");
                break;

            case TestState::TEST_PASSED:
                bg = spr_.color565(30, 150, 30);
                spr_.fillSprite(bg);
                spr_.setTextColor(TFT_WHITE, bg);
                spr_.setTextSize(3);
                spr_.setCursor(10, 10);
                spr_.printf("PASS");
                break;

            case TestState::TEST_FAILED:
                bg = spr_.color565((millis() % 1024 > 512) ? 200 : 100, 0, 0);
                spr_.fillSprite(bg);
                spr_.setTextColor(TFT_WHITE, bg);
                spr_.setTextSize(3);
                spr_.setCursor(10, 10);
                spr_.printf("FAIL!");
                break;

            default:
                assert(false);
        }

        spr_.pushSprite(0, 0);
        if (state_ != last_state) {
            last_state = state_;
            last_state_change = millis();

            if (state_ == TestState::TEST_PASSED) {
                mcp_.digitalWrite(MCP_PIN_BUZZER, HIGH);
                delay(50);
                mcp_.digitalWrite(MCP_PIN_BUZZER, LOW);
            } else if (state_ == TestState::TEST_FAILED) {
                mcp_.digitalWrite(MCP_PIN_BUZZER, HIGH);
                delay(2000);
                mcp_.digitalWrite(MCP_PIN_BUZZER, LOW);
            }
        }
        delay(10);
    }
}

void TesterTask::initializeIo() {
    pinMode(PIN_MOTOR_POWER, OUTPUT);
    pinMode(PIN_MOTOR_TEST_FEED, OUTPUT);
    pinMode(PIN_BUTTON_BOTTOM, INPUT_PULLUP);

    pinMode(PIN_MCP_NRESET, OUTPUT);
    digitalWrite(PIN_MCP_NRESET, HIGH);

    Wire.begin();
    Wire.setClock(400000);

    mcp_.begin(0, &Wire);
    ina219_.begin();
}

void TesterTask::initializeMcp() {
    mcp_.pinMode(MCP_PIN_CLAMP, INPUT);
    mcp_.pullUp(MCP_PIN_CLAMP, HIGH);

    mcp_.pinMode(MCP_PIN_BUZZER, OUTPUT);
    mcp_.digitalWrite(MCP_PIN_BUZZER, HIGH);
    delay(50);
    mcp_.digitalWrite(MCP_PIN_BUZZER, LOW);
}

void TesterTask::initializeDisplay() {
    tft_.begin();
    tft_.invertDisplay(1);
    tft_.setRotation(1);

    spr_.setColorDepth(16);
    spr_.createSprite(TFT_HEIGHT, TFT_WIDTH);
}

void TesterTask::abort(String message) {
    disablePower();
    state_ = TestState::TEST_ABORTED;
    Serial.printf("ABORT! %s", message.c_str());

    memset(motor_buffer, 0, MOTOR_BUFFER_LENGTH);
}

void TesterTask::pass() {
    disablePower();
    state_ = TestState::TEST_PASSED;
    Serial.printf("Passed!");

    memset(motor_buffer, 0, MOTOR_BUFFER_LENGTH);
}

void TesterTask::fail(String message) {
    disablePower();
    state_ = TestState::TEST_FAILED;
    Serial.printf("FAIL! %s", message.c_str());

    memset(motor_buffer, 0, MOTOR_BUFFER_LENGTH);
}

void TesterTask::disablePower() {
    digitalWrite(PIN_MOTOR_POWER, LOW);
    digitalWrite(PIN_MOTOR_TEST_FEED, LOW);
}
