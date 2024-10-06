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
#include <lwip/apps/sntp.h>

#include <WiFi.h>
#include <HTTPClient.h>

#include "config.h"
#include "firestore.h"
#include "firestore_test_reporter.h"
#include "jwt.h"
#include "secrets.h"
#include "tester_task.h"

#define PIN_MOTOR_POWER 13
#define PIN_MOTOR_TEST_FEED 27
#define PIN_MCP_NRESET 26
#define PIN_3V3_FUSED 36
#define PIN_BUTTON_BOTTOM 0

#define MCP_PIN_LED_SENSOR_0 0
#define MCP_PIN_CLAMP 12
#define MCP_PIN_BUZZER 11

// Number of modules being tested (different than NUM_MODULES since we only need to test 1 module on the chained board)
#define TEST_MODULES 7

#define TEST_SUITE_VERSION 9

using namespace json11;

TesterTask::TesterTask(SplitflapTask& splitflap_task, const uint8_t task_core) :
        Task{"Tester", 16000, 1, task_core},
        splitflap_task_{splitflap_task},
        jwt_("https://firestore.googleapis.com/", service_key_id, service_email, service_private_key, service_private_key_len),
        firestore_(project_id, jwt_),
        firestore_test_reporter_(firestore_) {
}

void TesterTask::disableHardware() {
    digitalWrite(PIN_MOTOR_POWER, LOW);
    digitalWrite(PIN_MOTOR_TEST_FEED, LOW);
}

void TesterTask::drawSimpleText(uint32_t foreground, uint32_t background, String title, String details, String bottom_button_label = "") {
    spr_.fillRect(0,0, TFT_HEIGHT, TFT_WIDTH, background);
    spr_.setTextColor(foreground);
    spr_.setTextDatum(TL_DATUM);

    uint8_t title_size = 5;
    if (title.length() > 12) {
        title_size = 2;
    } else if (title.length() > 7) {
        title_size = 3;
    }
    spr_.setTextSize(title_size);
    spr_.setCursor(0, 0);
    spr_.print(title.c_str());

    uint8_t detail_size = 3;
    if (details.length() > 30) {
        detail_size = 0;
    } else if (details.length() > 20) {
        detail_size = 1;
    } else if (details.length() > 12) {
        detail_size = 2;
    }
    spr_.setTextSize(detail_size);
    spr_.setCursor(0, title_size * 9);
    spr_.print(details.c_str());

    if (bottom_button_label.length() > 0) {
        spr_.setTextDatum(BR_DATUM);
        spr_.setTextSize(1);
        spr_.drawString(bottom_button_label, TFT_HEIGHT - 2, TFT_WIDTH - 2);
    }

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
        if (first_over_current_millis_ == 0) {
            first_over_current_millis_ = millis();
        } else if (millis() - first_over_current_millis_ > 50) {
            disableHardware();
            return Status::fatal("Over-current: " + String(current_));
        }
    } else {
        first_over_current_millis_ = 0;
    }
    clamped_ = !mcp_.digitalRead(MCP_PIN_CLAMP);

    button_bottom_pressed_ = !digitalRead(PIN_BUTTON_BOTTOM);

    return Status::ok();
}

Result TesterTask::doTestMaintenance() {
    Status status = doMaintenance();
    if (!status.isOk()) {
        return Result::abort(status.message_);
    }
    if (!clamped_) {
        disableHardware();
        return Result::fail("Board was removed during test!");
    }

    if (!digitalRead(PIN_3V3_FUSED)) {
        disableHardware();
        return Result::fail("3.3v failure");
    }

    return Result::pass("");
}

Result TesterTask::delayWithMaintenance(uint32_t delay_millis) {
    uint32_t start = millis();
    while (millis() - start < delay_millis) {
        Result maintenanceResult = doTestMaintenance();
        if (!maintenanceResult.canContinue()) {
            return maintenanceResult;
        }
    }
    return Result::pass("");
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

        if (clamped_ && !button_bottom_pressed_) {
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

Result TesterTask::readSerial() {
    Serial.println("Reading board serial...");

    for (uint8_t i = 0; i < 3; i++) {
        // Trigger scan
        Serial1.print("~T.");

        String buffer;
        bool got_ack = false;

        uint32_t start = millis();
        while (millis() - start < 1500) {
            Result maintenanceResult = doTestMaintenance();
            if (!maintenanceResult.canContinue()) {
                return maintenanceResult;
            }

            if (Serial1.available()) {
                int b = Serial1.read();
                if (!got_ack) {
                    if (b == 6) {
                        got_ack = true;
                    }
                } else {
                    if (b != -1) {
                        buffer.concat((char)b);
                        if (buffer.length() == String("ABC_CD12345_XYZ").length()) {
                            if (buffer.startsWith("ABC_CD") && buffer.endsWith("_XYZ")) {
                                return Result::pass(buffer.substring(4, 11));
                            }
                        }
                    }
                }
            }
        }
        Serial.println("Timed out waiting for serial from scanner");
    }
    return Result::fail("Unable to read serial number");
}

Result TesterTask::testLoopbacks() {
    SplitflapState splitflap_state = splitflap_task_.getState();
    if (splitflap_state.loopbacks_ok) {
        return Result::pass("Loopbacks are ok");
    } else {
        // TODO: Get more information about which loopback(s) failed and in what way
        disableHardware();
        return Result::fail("One or more bad loopback");
    }
}

Result TesterTask::testLeds() {
    String failure = "Led test failure(s):\n";
    bool success = true;
    for (uint8_t led = 0; led < 6; led++) {
        Result maintenanceResult = doTestMaintenance();
        if (!maintenanceResult.canContinue()) {
            return maintenanceResult;
        }

        testStatus("Checking LED " + String(led));
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

Result TesterTask::testPowerPreCheck() {
    uint32_t start = millis();
    while (1) {
        Result result = doTestMaintenance();
        if (!result.canContinue()) {
            return result;
        }

        if (voltage_ < 1.5) {
            break;
        }
        testStatus("Waiting for voltage to drop... V=" + String(voltage_));
        if (millis() - start > 10000) {
            disableHardware();
            return Result::fail("Timeout waiting for power to reset");
        }
        delay(100);
    }

    testStatus("Checking test feed");
    digitalWrite(PIN_MOTOR_TEST_FEED, HIGH);

    start = millis();
    while (1) {
        Result result = doTestMaintenance();
        if (!result.canContinue()) {
            return result;
        }

        testStatus("Checking test feed\n" + String(voltage_) + "V\n" + String(current_) + "mA");

        if (voltage_ > 3.3) {
            disableHardware();
            return Result::abort("Over-voltage during test feed: " + String(voltage_));
        }
        if (millis() - start > 15000) {
            disableHardware();
            return Result::fail("Timeout waiting for power test feed. Current voltage: " + String(voltage_));
        }
        if (millis() - start > 400 && current_ > 15) {
            disableHardware();
            return Result::fail("Over-current during test feed: " + String(current_));
        }
        if (millis() - start > 400 && voltage_ > 2.5) {
            digitalWrite(PIN_MOTOR_TEST_FEED, LOW);
            return Result::pass("Power pre-check OK. " + String(voltage_) + "V, " + String(current_) + "mA");
        }

        delay(10);
    }
}

Result TesterTask::testPower() {
    testStatus("Checking power");
    digitalWrite(PIN_MOTOR_POWER, HIGH);

    uint32_t start = millis();
    while (1) {
        Result result = doTestMaintenance();
        if (!result.canContinue()) {
            return result;
        }

        testStatus("Checking power\n" + String(voltage_) + "V\n" + String(current_) + "mA");

        if (millis() - start > 5000) {
            disableHardware();
            return Result::fail("Timeout waiting for power");
        }
        if (millis() - start > 500 && current_ > 10) {
            disableHardware();
            return Result::fail("Over-current during test: " + String(current_));
        }
        if (millis() - start > 500 && voltage_ > 11.8) {
            return Result::pass("Power OK. V=" + String(voltage_) + "v, A=" + String(current_) + "ma");
        }

        delay(10);
    }
}

Result TesterTask::testHoming() {
    testStatus("Homing...");
    splitflap_task_.resetAll();

    // Delay to make sure we don't read cached state from before the reset
    delay(100);

    uint32_t start = millis();

    while (1) {
        Result result = doTestMaintenance();
        if (!result.canContinue()) {
            return result;
        }

        SplitflapState splitflap_state = splitflap_task_.getState();
        bool ready = true;
        String description = "Homing...\n";
        for (uint8_t i = 0; i < TEST_MODULES; i++) {
            bool module_ready = splitflap_state.modules[i].state == NORMAL
                    && splitflap_state.modules[i].count_missed_home == 0
                    && splitflap_state.modules[i].count_unexpected_home == 0
                    && splitflap_state.modules[i].moving == false;
            ready &= module_ready;
            description += String(i) + ": ";
            if (ready) {
                description += "OK\n";
            } else {
                description += "...\n";
            }
        }
        description += String(current_);
        testStatus(description);

        uint32_t duration = millis() - start;

        if (ready && duration > 1000) {
            return Result::pass("All modules homed successfully");
        } else if (duration > 1000 && duration < 20000) {
            // Failure if all modules have stopped moving but aren't all ready (checked above)
            // Note: we could fail sooner (if any module has an error state), but it is more useful to identify
            // *all* module failures instead of just the first.
            bool any_moving = false;
            for (uint8_t i = 0; i < TEST_MODULES; i++) {
                any_moving |= splitflap_state.modules[i].moving;
            }
            if (!any_moving) {
                String failure = "Some modules failed home calibration:\n";
                for (uint8_t i = 0; i < TEST_MODULES; i++) {
                    if (splitflap_state.modules[i].state != NORMAL || splitflap_state.modules[i].count_missed_home != 0 || splitflap_state.modules[i].count_unexpected_home != 0) {
                        failure.concat("Module ");
                        failure.concat(i);
                        if (splitflap_state.modules[i].state != NORMAL) {
                            failure.concat(" is in incorrect state ");
                            failure.concat(splitflap_state.modules[i].state);
                        }
                        if (splitflap_state.modules[i].count_missed_home != 0 || splitflap_state.modules[i].count_unexpected_home != 0) {
                            failure.concat(" has errors: missed_home=");
                            failure.concat(splitflap_state.modules[i].count_missed_home);
                            failure.concat(" unexpected_home=");
                            failure.concat(splitflap_state.modules[i].count_unexpected_home);
                        }
                        failure.concat("\n");
                    }
                }
                return Result::fail(failure);
            }
        } else if (duration > 20000) {
            return Result::fail("Timeout waiting for home calibration to complete.");
        }
        delay(100);
    }
}

Result TesterTask::testMovements() {
    char test_string_a[TEST_MODULES];
    char test_string_b[TEST_MODULES];
    for (uint8_t i = 0; i < TEST_MODULES; i++) {
        test_string_a[i] = 'd' + i*2;
        test_string_b[i] = 'd' + TEST_MODULES*2 - i*2;
    }

    const uint8_t movements = 6;

    for (uint8_t movement = 0; movement < movements; movement++) {
        uint32_t start = millis();
        splitflap_task_.showString(((movement % 2) == 0) ? test_string_a : test_string_b, TEST_MODULES);
        testStatus("Running iteration " + String(movement));

        while (1) {
            Result result = doTestMaintenance();
            if (!result.canContinue()) {
                return result;
            }

            SplitflapState splitflap_state = splitflap_task_.getState();
            bool ready = true;
            for (uint8_t i = 0; i < TEST_MODULES; i++) {
                ready &= splitflap_state.modules[i].state == NORMAL
                        && splitflap_state.modules[i].count_missed_home == 0
                        && splitflap_state.modules[i].count_unexpected_home == 0
                        && splitflap_state.modules[i].moving == false;
            }

            uint32_t duration = millis() - start;

            if (ready) {
                break;
            } else if (duration > 1000 && duration < 10000) {
                // Failure if all modules have stopped moving but aren't all ready (checked above)
                // Note: we could fail sooner (if any module has an error state), but it is more useful to identify
                // *all* module failures instead of just the first.
                bool any_moving = false;
                for (uint8_t i = 0; i < TEST_MODULES; i++) {
                    any_moving |= splitflap_state.modules[i].moving;
                }
                if (!any_moving) {
                    String failure = "Some modules failed movement on iteration " + String(movement) + ":\n";
                    for (uint8_t i = 0; i < TEST_MODULES; i++) {
                        if (splitflap_state.modules[i].state != NORMAL || splitflap_state.modules[i].count_missed_home != 0 || splitflap_state.modules[i].count_unexpected_home != 0) {
                            failure.concat("Module ");
                            failure.concat(i);
                            if (splitflap_state.modules[i].state != NORMAL) {
                                failure.concat(" is in incorrect state ");
                                failure.concat(splitflap_state.modules[i].state);
                            }
                            if (splitflap_state.modules[i].count_missed_home != 0 || splitflap_state.modules[i].count_unexpected_home != 0) {
                                failure.concat(" has errors: missed_home=");
                                failure.concat(splitflap_state.modules[i].count_missed_home);
                                failure.concat(" unexpected_home=");
                                failure.concat(splitflap_state.modules[i].count_unexpected_home);
                            }
                            failure.concat("\n");
                        }
                    }
                    return Result::fail(failure);
                }
            } else if (duration > 10000) {
                return Result::fail("Timeout waiting for movement to complete on iteration " + String(movement) + ".");
            }
            delay(50);
        }
        delay(50);
    }
    return Result::pass("All modules completed " + String(movements) + " iterations successfully");
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
                    return Status::fatal("Over-current during startup: " + String(current_));
                } else {
                    // Good voltage and current; disable motor power for now and continue with startup
                    digitalWrite(PIN_MOTOR_POWER, LOW);
                    return Status::ok();
                }
            }
        }
    }
}

void TesterTask::connectWifi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        esp_err_t result = esp_task_wdt_reset();
        ESP_ERROR_CHECK(result);

        String waitString = "Wifi connecting";
        uint8_t dots = (millis() / 200) % 4;
        for (uint8_t i = 0; i < 4; i++) {
            if (i < dots) {
                waitString += ".";
            } else {
                waitString += " ";
            }
        }
        drawSimpleText(TFT_WHITE, TFT_ORANGE, "Starting", waitString);

        delay(1000);
    }
}

void TesterTask::syncTime() {
    sntp_setoperatingmode(SNTP_OPMODE_POLL);

    char server[] = "time.nist.gov"; // sntp_setservername takes a non-const char*, so use a non-const variable to avoid warning
    sntp_setservername(0, server);
    sntp_init();
    time_t now = 0;
    while(time(&now), now < 1625099485) {
        esp_err_t result = esp_task_wdt_reset();
        ESP_ERROR_CHECK(result);

        String waitString = "Syncing time";
        uint8_t dots = (millis() / 200) % 4;
        for (uint8_t i = 0; i < 4; i++) {
            if (i < dots) {
                waitString += ".";
            } else {
                waitString += " ";
            }
        }
        drawSimpleText(TFT_WHITE, TFT_ORANGE, "Starting", waitString);
        delay(1000);
    }

    char buf[20];
    strftime(buf, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
    Serial.printf("Got time: %s\n", buf);
}

void TesterTask::run() {
    esp_err_t result = esp_task_wdt_add(NULL);
    ESP_ERROR_CHECK(result);

    initializeIo();
    initializeMcp();
    ina219_.setCalibrationSplitflap();
    initializeDisplay();

    drawSimpleText(TFT_WHITE, TFT_PURPLE, "Tester", "v" + String(TEST_SUITE_VERSION));
    delay(1500);

    Status status = runTestSuitesForever();
    if (!status.isOk()) {
        disableHardware();  // Should have already been called by whoever threw fatal error, but disable again just to be safe
        drawSimpleText(TFT_WHITE, TFT_RED, "FATAL ERROR", status.message_);
    }

    while(1) {
        esp_err_t result = esp_task_wdt_reset();
        ESP_ERROR_CHECK(result);
        delay(10);
    }
}

Result TesterTask::runTestSuite() {
    {
        testStarted("Loopbacks");
        Result result = testLoopbacks();
        testFinished(result);

        if (!result.canContinue()) {
            return result;
        }
    }

    {
        testStarted("LEDs");
        Result result = testLeds();
        testFinished(result);

        if (!result.canContinue()) {
            return result;
        }
    }

    {
        testStarted("Power pre-check");
        Result result = testPowerPreCheck();
        testFinished(result);

        if (!result.canContinue()) {
            return result;
        }
    }

    {
        testStarted("Power");
        Result result = testPower();
        testFinished(result);

        if (!result.canContinue()) {
            return result;
        }
    }

    {
        testStarted("Homing");
        Result result = testHoming();
        testFinished(result);

        if (!result.canContinue()) {
            return result;
        }
    }

    {
        testStarted("Movements");
        Result result = testMovements();
        testFinished(result);

        if (!result.canContinue()) {
            return result;
        }
    }

    // More tests...

    return Result::pass("All tests passed");
}

Status TesterTask::runTestSuitesForever() {
    connectWifi();
    syncTime();
    
    drawSimpleText(TFT_WHITE, TFT_ORANGE, "Starting", "Checking Firestore access...");
    esp_err_t result = esp_task_wdt_delete(NULL);
    ESP_ERROR_CHECK(result);
    if (!firestore_test_reporter_.checkFirestoreAccess()) {
        return Status::fatal("Failed Firestore startup self-test!");
    }
    result = esp_task_wdt_add(NULL);
    ESP_ERROR_CHECK(result);


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

        String serial;
        {
            Result result = readSerial();
            if (result.result_code_ == Result::Code::ABORT) {
                return Status::fatal(result.message_);
            } else if (result.result_code_ == Result::Code::FAIL) {
                continue;
            } else {
                serial = result.message_;
            }
        }

        drawSimpleText(TFT_WHITE, TFT_BLACK, "Serial:", serial);
        delay(1000);

        testSuiteStarted(serial);
        Result result = runTestSuite();
        disableHardware();

        testSuiteFinished(result.result_code_);

        switch (result.result_code_) {
            case Result::Code::ABORT:
                return Status::fatal(result.message_);
            case Result::Code::PASS:
                drawSimpleText(TFT_WHITE, tft_.color565(30, 150, 30), "PASS", result.message_, "Hold to re-run");
                for (uint8_t i = 0; i < 2; i++) {
                    mcp_.digitalWrite(MCP_PIN_BUZZER, HIGH);
                    delay(100);
                    mcp_.digitalWrite(MCP_PIN_BUZZER, LOW);
                    delay(150);
                }
                break;
            case Result::Code::FAIL:
                drawSimpleText(TFT_WHITE, TFT_RED, "FAIL", result.message_, "Hold to re-run");
                mcp_.digitalWrite(MCP_PIN_BUZZER, HIGH);
                delay(1500);
                mcp_.digitalWrite(MCP_PIN_BUZZER, LOW);
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
}

void TesterTask::initializeIo() {
    pinMode(PIN_MOTOR_POWER, OUTPUT);
    pinMode(PIN_MOTOR_TEST_FEED, OUTPUT);
    pinMode(PIN_BUTTON_BOTTOM, INPUT_PULLUP);
    pinMode(PIN_3V3_FUSED, INPUT);

    pinMode(PIN_MCP_NRESET, OUTPUT);
    digitalWrite(PIN_MCP_NRESET, HIGH);

    Wire.begin();
    Wire.setClock(400000);

    Serial1.begin(9600, SERIAL_8N1, 38, 15);

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


void TesterTask::testSuiteStarted(String serial) {
    Serial.printf("Test suite started: %s\n", serial.c_str());
    test_suite_start_millis_ = millis();
    firestore_test_reporter_.testSuiteStarted(serial, TEST_SUITE_VERSION);
}

bool TesterTask::testSuiteFinished(Result::Code result_code) {
    uint32_t test_suite_duration = millis() - test_suite_start_millis_;
    Serial.printf("Test suite finished in %u millis\n", test_suite_duration);
    test_suite_start_millis_ = 0;
    current_test_id_ = "";

    // Report to Firestore
    drawSimpleText(TFT_WHITE, TFT_BLACK, "Saving...", "Recording results to Firestore...");
    esp_err_t result = esp_task_wdt_delete(NULL);
    ESP_ERROR_CHECK(result);
    bool success = firestore_test_reporter_.testSuiteFinished(result_code);
    result = esp_task_wdt_add(NULL);
    ESP_ERROR_CHECK(result);

    return success;
}

void TesterTask::testStarted(String id) {
    Serial.printf("Test started: %s\n", id.c_str());
    current_test_id_ = id;
    test_start_millis_ = millis();
    drawSimpleText(TFT_WHITE, TFT_BLACK, "Test: " + current_test_id_, "Started");
    firestore_test_reporter_.testStarted(id);
}
void TesterTask::testFinished(Result result) {
    uint32_t test_duration = millis() - test_start_millis_;
    Serial.printf("Test took %u millis. Result: %u -> %s\n", test_duration, result.result_code_, result.message_.c_str());
    test_start_millis_ = 0;
    current_test_id_ = "";
    firestore_test_reporter_.testFinished(result);
}

void TesterTask::testStatus(String message) {
    Serial.println(message);
    drawSimpleText(TFT_WHITE, TFT_BLACK, "Test: " + current_test_id_, message);
}
