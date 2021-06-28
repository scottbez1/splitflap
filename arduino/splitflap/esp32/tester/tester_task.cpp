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
#include <esp_task_wdt.h>

#include "config.h"

#include "tester_task.h"

#define PIN_MOTOR_POWER 13
#define PIN_MOTOR_TEST_FEED 27
#define PIN_MCP_NRESET 26
#define PIN_BUTTON_BOTTOM 0

#define MCP_PIN_LED_SENSOR_0 0
#define MCP_PIN_CLAMP 6
#define MCP_PIN_BUZZER 11

// Number of modules being tested (different than NUM_MODULES since we only need to test 1 module on the chained board)
#define TEST_MODULES 7

TesterTask::TesterTask(SplitflapTask& splitflap_task, const uint8_t task_core) : Task{"Tester", 8192, 1, task_core}, splitflap_task_{splitflap_task} {
}

void TesterTask::disableHardware() {
    digitalWrite(PIN_MOTOR_POWER, LOW);
    digitalWrite(PIN_MOTOR_TEST_FEED, LOW);
}

void TesterTask::drawSimpleText(uint32_t foreground, uint32_t background, String title, String details) {
    spr_.fillRect(0,0, TFT_HEIGHT, TFT_WIDTH, background);
    spr_.setTextColor(foreground);

    spr_.setTextSize(2);
    spr_.setCursor(0, 0);
    spr_.printf(title.c_str());

    spr_.setTextSize(0);
    spr_.setCursor(0, 30);
    spr_.printf(details.c_str());

    spr_.pushSprite(0, 0);
}

Status TesterTask::doMaintenance() {
    esp_err_t result = esp_task_wdt_reset();
    ESP_ERROR_CHECK(result);

    voltage_ = ina219_.getBusVoltage_V();
    current_ = ina219_.getCurrent_mA();
    if (voltage_ > 13) {
        disableHardware();
        return Status::fatal("Over-voltage: " + String(voltage_));
    } else if (current_ > 2000) {
        disableHardware();
        return Status::fatal("Over-current: " + String(current_));
    }

    // TODO: check 3v3_fused status

    clamped_ = !mcp_.digitalRead(MCP_PIN_CLAMP);

    bool new_button_state_bottom = !digitalRead(PIN_BUTTON_BOTTOM);
    // TODO: debounce

    return Status::ok();
}

Result TesterTask::doTestMaintenance() {
    Status status = doMaintenance();
    if (!status.isOk()) {
        return Result::abort(status.message_);
    }
    if (!clamped_) {
        disableHardware();
        return Result::abort("Board was removed during test!");
    }
}

Status TesterTask::waitForBoardInserted() {
    Serial.println("Waiting for board to be inserted...");
    uint32_t time_clamped = 0;

    while (1) {
        Status status = doMaintenance();
        if (!status.isOk()) {
            return status;
        }

        if (!clamped_) {
            drawSimpleText(TFT_WHITE, COLOR_ACTION, "Insert board", "");
            time_clamped = 0;
        } else {
            drawSimpleText(TFT_WHITE, TFT_BLACK, "Please wait...", "");
            if (time_clamped == 0) {
                time_clamped = millis();
            } else if (millis() - time_clamped > 1500) {
                return Status::ok();
            }
        }
    }
}

Status TesterTask::waitForBoardRemoved() {
    Serial.println("Waiting for board to be removed...");
    uint32_t time_unclamped = 0;

    while (1) {
        Status status = doMaintenance();
        if (!status.isOk()) {
            return status;
        }

        if (clamped_) {
            time_unclamped = 0;
        } else {
            if (time_unclamped == 0) {
                time_unclamped = millis();
            } else if (millis() - time_unclamped > 1500) {
                return Status::ok();
            }
        }
    }
}

String TesterTask::readSerial() {
    Serial.println("Reading board serial...");
    return "DUMMY";
}

Result TesterTask::testLeds() {
    reporter_.testStarted("leds");
    const String test_name = "Test: LEDs";

    drawSimpleText(TFT_WHITE, TFT_BLACK, test_name, "");

    String failure = "Led test failure(s):\n";
    bool success = true;
    for (uint8_t led = 0; led < 6; led++) {
        Result maintenanceResult = doTestMaintenance();
        if (!maintenanceResult.canContinue()) {
            return maintenanceResult;
        }

        Serial.printf("Setting LED %u\n", led);
        drawSimpleText(TFT_WHITE, TFT_BLACK, test_name, "LED " + String(led));
        for (uint8_t i = 0; i < 6; i++) {
            splitflap_task_.setLed(i, led == i);
        }
        delay(20);
        for (uint8_t i = 0; i < 6; i++) {
            bool detected = !mcp_.digitalRead(MCP_PIN_LED_SENSOR_0 + i);
            if (detected && i != led) {
                failure.concat("LED ");
                failure.concat(led);
                failure.concat(" is lit, but falsely detected LED ");
                failure.concat(i);
                failure.concat("\n");
                success = false;
            } else if (!detected && i == led) {
                failure.concat("LED ");
                failure.concat(led);
                failure.concat(" is lit, but was not detected\n");
                success = false;
            }
            delay(10);
        }
    }
    for (uint8_t i = 0; i < 6; i++) {
        splitflap_task_.setLed(i, 0);
    }

    if (success) {
        return Result::pass("All LEDs ok");
    } else {
        disableHardware();
        return Result::fail(failure);
    }
}

Status TesterTask::runStartupSelfTest() {
    // Check power on startup
    drawSimpleText(TFT_WHITE, TFT_BLACK, "Starting", "Waiting for power...");

    digitalWrite(PIN_MOTOR_POWER, HIGH);
    uint32_t voltage_detected_millis = 0;
    while (1) {
        Status maintenanceResult = doMaintenance();
        if (!maintenanceResult.isOk()) {
            return maintenanceResult;
        }
        if (voltage_ > 11.8) {
            if (voltage_detected_millis == 0) {
                voltage_detected_millis = millis();
            } else if (millis() - voltage_detected_millis > 200) {
                if (current_ > 10) {
                    disableHardware();
                    return Status::fatal("Over-current: " + String(current_));
                } else {
                    // Good voltage and current; disable motor power for now and continue with startup
                    digitalWrite(PIN_MOTOR_POWER, LOW);
                    return Status::ok();
                }
            }
        }
    }
}

void TesterTask::run() {
    esp_err_t result = esp_task_wdt_add(NULL);
    ESP_ERROR_CHECK(result);

    initializeIo();
    initializeMcp();
    ina219_.setCalibrationSplitflap();
    initializeDisplay();

    Status status = runTestSuitesForever();
    if (!status.isOk()) {
        disableHardware();  // Should be called by whoever threw fatal error, but disable again just to be safe
        drawSimpleText(TFT_WHITE, TFT_RED, "FATAL ERROR", status.message_);
    }
}

Result TesterTask::runTestSuite() {
    reporter_.testStarted("LEDs");
    Result result = testLeds();
    reporter_.testFinished(result);

    if (!result.canContinue()) {
        return result;
    }

    // More tests...

    return Result::pass("All tests passed");
}

Status TesterTask::runTestSuitesForever() {

    drawSimpleText(TFT_WHITE, COLOR_ACTION, "Remove board", "");
    {
        Status status = waitForBoardRemoved();
        if (!status.isOk()) {
            return status;
        }
    }

    Status status = runStartupSelfTest();
    if (!status.isOk()) {
        return status;
    }

    while (1) {
        {
            Status status = waitForBoardInserted();
            if (!status.isOk()) {
                return status;
            }
        }

        String serial = readSerial();
        if (serial.length() == 0) {
            continue;
        }

        reporter_.testSuiteStarted(serial);
        Result result = runTestSuite();
        reporter_.testSuiteFinished();
        switch (result.result_code_) {
            case Result::Code::ABORT:
                return Status::fatal(result.message_);
            case Result::Code::PASS:
                drawSimpleText(TFT_WHITE, tft_.color565(30, 150, 30), "PASS", result.message_);
                break;
            case Result::Code::FAIL:
                drawSimpleText(TFT_WHITE, TFT_RED, "FAIL", result.message_);
                break;
        }
        
        {
            Status status = waitForBoardRemoved();
            if (!status.isOk()) {
                return status;
            }
        }
    }
    
    return Status::ok();

        // TODO: move loopback testing to splitflap_task; just reset it then monitor it here

        // spr_.setTextColor(TFT_WHITE, TFT_BLACK);
        // spr_.setTextSize(2);
        // spr_.setCursor(0, 0);
        // spr_.printf("Testing loopback...");
        // bool loopback_result[NUM_LOOPBACKS][NUM_LOOPBACKS];
        // bool loopback_off_result[NUM_LOOPBACKS];
        // bool loopback_success = splitflap_task_.testAllLoopbacks(loopback_result, loopback_off_result);

        // if (!loopback_success) {
        //     String failure;
        //     for (uint8_t i = 0; i < NUM_LOOPBACKS; i++) {
        //         for (uint8_t j = 0; j < NUM_LOOPBACKS; j++) {
        //             if (!loopback_result[i][j]) {
        //                 failure.concat("Bad loopback. Set output ");
        //                 failure.concat(i);
        //                 failure.concat(" but read incorrect value at input ");
        //                 failure.concat(j);
        //                 failure.concat("\n");
        //             }
        //         }
        //     }
        //     for (uint8_t j = 0; j < NUM_LOOPBACKS; j++) {
        //         if (!loopback_off_result[j]) {
        //             failure.concat("Bad loopback at input ");
        //             failure.concat(j);
        //             failure.concat(" when all outputs off - should have been 0\n");
        //         }
        //     }
        //     fail(failure);
        // } else {
        // }

    //             break;
    //         }
    //         case TestState::TEST_CHECK_MOTOR_POWER:
    //             spr_.setTextColor(TFT_WHITE, TFT_BLACK);
    //             spr_.setTextSize(2);
    //             spr_.setCursor(0, 0);
    //             spr_.printf("Preparing power...");
    //             if (voltage > 3.3) {
    //                 fail("Over-voltage during test feed: " + String(voltage));
    //             } else if (time_in_state > 200 && current > 10) {
    //                 fail("Over-current during test feed: " + String(current));
    //             } else if (time_in_state > 200 && voltage > 2.5) {
    //                 state_ = TestState::TEST_ENABLE_MOTOR_POWER;
    //                 digitalWrite(PIN_MOTOR_TEST_FEED, LOW);
    //                 digitalWrite(PIN_MOTOR_POWER, HIGH);
    //             } else if (time_in_state > 1000) {
    //                 fail("Timeout waiting for power");
    //             }
    //             break;
    //         case TestState::TEST_ENABLE_MOTOR_POWER:
    //             spr_.setTextColor(TFT_WHITE, TFT_BLACK);
    //             spr_.setTextSize(2);
    //             spr_.setCursor(0, 0);
    //             spr_.printf("Checking power...");
    //             if (voltage > 11.8) {
    //                 state_ = TestState::TEST_HOME_ALL;
    //                 splitflap_task_.resetAll();
    //             } else if (time_in_state > 200 && current > 10) {
    //                 fail("Over-current: " + String(current));
    //             } else if (time_in_state > 1000) {
    //                 fail("Timeout waiting for power");
    //             }
    //             break;
            
    //         case TestState::TEST_HOME_ALL: {
    //             spr_.setTextColor(TFT_WHITE, TFT_BLACK);
    //             spr_.setTextSize(2);
    //             spr_.setCursor(0, 0);
    //             spr_.printf("Home calibration...");

    //             bool ready = true;
    //             SplitflapState splitflap_state = splitflap_task_.getState();
    //             for (uint8_t i = 0; i < TEST_MODULES; i++) {
    //                 // TODO: also check error counters
    //                 ready &= splitflap_state.modules[i].state == NORMAL;
    //             }

    //             if (ready) {
    //                 pass();
    //             } else if (time_in_state > 1000 && time_in_state < 20000) {
    //                 // Failure if all modules have stopped moving but aren't all ready (checked above)
    //                 // Note: we could fail sooner (if any module has an error state), but it is more useful to identify
    //                 // *all* module failures instead of just the first.
    //                 bool any_moving = false;
    //                 for (uint8_t i = 0; i < TEST_MODULES; i++) {
    //                     any_moving |= splitflap_state.modules[i].moving;
    //                 }
    //                 if (!any_moving) {
    //                     String failure = "Some modules failed calibration:\n";
    //                     for (uint8_t i = 0; i < TEST_MODULES; i++) {
    //                         if (splitflap_state.modules[i].state != NORMAL) {
    //                             failure.concat("Module ");
    //                             failure.concat(i);
    //                             failure.concat(" is in incorrect state ");
    //                             failure.concat(splitflap_state.modules[i].state);
    //                             failure.concat("\n");
    //                         }
    //                     }
    //                     fail(failure);
    //                 }
    //             } else if (time_in_state > 20000) {
    //                 fail("Timeout waiting for home calibration to complete");
    //             }
    //             break;
    //         }
    //         case TestState::RESULT_ABORTED:
    //             bg = spr_.color565(200, 25, 0);
    //             spr_.fillSprite(bg);
    //             spr_.setTextColor(TFT_WHITE, bg);
    //             spr_.setTextSize(3);
    //             spr_.setCursor(0, 0);
    //             spr_.printf("Aborted!");
    //             spr_.setTextSize(0);
    //             spr_.setCursor(0, 30);
    //             spr_.printf(message_.c_str());
    //             break;

    //         case TestState::RESULT_PASSED:
    //             bg = spr_.color565(30, 150, 30);
    //             spr_.fillSprite(bg);
    //             spr_.setTextColor(TFT_WHITE, bg);
    //             spr_.setTextSize(3);
    //             spr_.setCursor(0, 0);
    //             spr_.printf("PASS");
    //             break;

    //         case TestState::RESULT_FAILED:
    //             bg = spr_.color565((millis() % 1024 > 512) ? 200 : 100, 0, 0);
    //             spr_.fillSprite(bg);
    //             spr_.setTextColor(TFT_WHITE, bg);
    //             spr_.setTextSize(3);
    //             spr_.setCursor(0, 0);
    //             spr_.printf("FAIL!");
    //             spr_.setTextSize(0);
    //             spr_.setCursor(0, 30);
    //             spr_.printf(message_.c_str());
    //             break;

    //         default:
    //             assert(false);
    //     }

    //     if (state_ != last_state) {
    //         last_state = state_;
    //         last_state_change = millis();

    //         if (state_ == TestState::RESULT_PASSED) {
    //             mcp_.digitalWrite(MCP_PIN_BUZZER, HIGH);
    //             delay(50);
    //             mcp_.digitalWrite(MCP_PIN_BUZZER, LOW);
    //         } else if (state_ == TestState::RESULT_FAILED) {
    //             mcp_.digitalWrite(MCP_PIN_BUZZER, HIGH);
    //             delay(800);
    //             mcp_.digitalWrite(MCP_PIN_BUZZER, LOW);
    //         }
    //     }
    //     delay(10);
    // }
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

    for (uint8_t i = MCP_PIN_LED_SENSOR_0; i < MCP_PIN_LED_SENSOR_0 + 6; i++) {
        mcp_.pinMode(i, INPUT);
        mcp_.pullUp(i, HIGH);
    }

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
