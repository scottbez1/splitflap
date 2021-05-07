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
#define PIN_BUTTON_1 0

#define MCP_PIN_BUZZER 11

TesterTask::TesterTask(const uint8_t task_core) : Task{"Tester", 8192, 1, task_core} {

}

void TesterTask::run() {
    pinMode(PIN_MOTOR_POWER, OUTPUT);
    pinMode(PIN_MOTOR_TEST_FEED, OUTPUT);
    pinMode(PIN_BUTTON_1, INPUT_PULLUP);

    pinMode(PIN_MCP_NRESET, OUTPUT);
    digitalWrite(PIN_MCP_NRESET, HIGH);

    Wire.begin();
    Wire.setClock(400000);

    mcp_.begin(0, &Wire);
    ina219_.begin();

    initialize_modules();

    mcp_.pinMode(MCP_PIN_BUZZER, OUTPUT);
    mcp_.digitalWrite(MCP_PIN_BUZZER, HIGH);
    delay(50);
    mcp_.digitalWrite(MCP_PIN_BUZZER, LOW);

    ina219_.setCalibrationSplitflap();

    while (1) {
        delay(1);
        if (digitalRead(PIN_BUTTON_1)) {
            continue;
        }
        if (!enablePower()) {
            continue;
        };


        delay(4000);
        Serial.println("Stopping!");
        disablePower();
    }
}

bool TesterTask::enablePower() {
    Serial.println("Beginning power-on sequence...");

    float voltage = ina219_.getBusVoltage_V();
    float current = ina219_.getCurrent_mA();
    Serial.printf("  %.2fV\t %.1fmA\n", voltage, current);
    if (voltage > 1 || current > 20) {
        Serial.printf("  Bad power state!");
        disablePower();
        return false;
    }
    Serial.println("  Good!\n");

    Serial.println("  Enable test feed");
    digitalWrite(PIN_MOTOR_TEST_FEED, HIGH);

    // Wait for capacitors to charge
    delay(3000);

    voltage = ina219_.getBusVoltage_V();
    current = ina219_.getCurrent_mA();
    Serial.printf("    %.2fV\t %.1fmA\n", voltage, current);
    if (voltage < 2 || voltage > 3.2 || current > 20) {
        Serial.printf("    Bad power state with test feed!");
        disablePower();
        mcp_.digitalWrite(MCP_PIN_BUZZER, HIGH);
        delay(2000);
        mcp_.digitalWrite(MCP_PIN_BUZZER, LOW);
        return false;
    }
    Serial.println("    Good!\n");
    digitalWrite(PIN_MOTOR_TEST_FEED, LOW);

    delay(100);

    Serial.println("  Enable 12V input");
    digitalWrite(PIN_MOTOR_POWER, HIGH);
    delay(5);
    for (uint8_t i = 0; i < 10; i++) {
        voltage = ina219_.getBusVoltage_V();
        current = ina219_.getCurrent_mA();
        Serial.printf("    %.2fV\t %.1fmA\n", voltage, current);
        if (voltage > 13 || voltage < 2 || current > 500) {
            Serial.printf("    Bad intermediate power state with 12V enabled!");
            disablePower();
            mcp_.digitalWrite(MCP_PIN_BUZZER, HIGH);
            delay(2000);
            mcp_.digitalWrite(MCP_PIN_BUZZER, LOW);
            return false;
        }
        delay(10);
    }

    voltage = ina219_.getBusVoltage_V();
    current = ina219_.getCurrent_mA();
    Serial.printf("    %.2fV\t %.1fmA\n", voltage, current);
    if (voltage < 12 - 0.5 || voltage > 12 + 0.5 || current > 20) {
        Serial.printf("    Bad final power state with 12V enabled!");
        disablePower();
        mcp_.digitalWrite(MCP_PIN_BUZZER, HIGH);
        delay(2000);
        mcp_.digitalWrite(MCP_PIN_BUZZER, LOW);
        return false;
    }
    Serial.println("    Good!\n");


    return true;
}

void TesterTask::disablePower() {
    digitalWrite(PIN_MOTOR_POWER, LOW);
    digitalWrite(PIN_MOTOR_TEST_FEED, LOW);
}
