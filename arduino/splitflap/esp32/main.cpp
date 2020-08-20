/*
   Copyright 2020 Scott Bezek and the splitflap contributors

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

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Wire.h>

#include "config.h"
#include "display.h"
#include "src/splitflap_module.h"
#include "src/spi_io_config.h"

#define FAVR(x) x

Display display;

int recv_buffer[NUM_MODULES];

void dump_status(void);

void setup() {
  Serial.begin(MONITOR_SPEED);

  initialize_modules();
  motor_sensor_io();

  // Wait until after 1 motor_sensor_io to turn on shift register outputs
  pinMode(OUTPUT_ENABLE_PIN, OUTPUT);
  digitalWrite(OUTPUT_ENABLE_PIN, LOW);

  Serial.print("\n\n\n");
  Serial.print(FAVR("{\"type\":\"init\", \"num_modules\":"));
  Serial.print(NUM_MODULES);
  Serial.print(FAVR("}\n"));

  for (uint8_t i = 0; i < NUM_MODULES; i++) {
    recv_buffer[i] = 0;
    modules[i]->Init();
#if !SENSOR_TEST
    modules[i]->GoHome();
#endif
  }

  display.begin();
}


inline int8_t FindFlapIndex(uint8_t character) {
    for (int8_t i = 0; i < NUM_FLAPS; i++) {
        if (character == flaps[i]) {
          return i;
        }
    }
    return -1;
}

bool pending_move_response = true;
bool pending_no_op = false;
uint8_t recv_count = 0;

bool disabled = false;

void disableAll(char* message) {
  for (uint8_t i = 0; i < NUM_MODULES; i++) {
    modules[i]->Disable();
  }
  motor_sensor_io();

  if (disabled) {
    return;
  }
  disabled = true;

  Serial.println("#### DISABLED! ####");
  Serial.println(message);
}

boolean was_stopped = false;
uint32_t stopped_at_millis = 0;

inline void run_iteration() {
    uint32_t iterationStartMillis = millis();
    boolean all_idle = true;
    boolean all_stopped = true;
    for (uint8_t i = 0; i < NUM_MODULES; i++) {
      modules[i]->Update();
      bool is_idle = modules[i]->state == PANIC
        || modules[i]->state == STATE_DISABLED
        || modules[i]->state == LOOK_FOR_HOME
        || modules[i]->state == SENSOR_ERROR
        || (modules[i]->state == NORMAL && modules[i]->current_accel_step == 0);

      bool is_stopped = modules[i]->state == PANIC
        || modules[i]->state == STATE_DISABLED
        || modules[i]->current_accel_step == 0;

      all_idle &= is_idle;
      all_stopped &= is_stopped;
      // if (i & 0b11) motor_sensor_io();
    }
    if (all_stopped && !was_stopped) {
      stopped_at_millis = iterationStartMillis;
    }
    was_stopped = all_stopped;
    motor_sensor_io();

#if INA219_POWER_SENSE
    if (iterationStartMillis - lastCurrentReadMillis > 100) {
      currentmA = powerSense.getCurrent_mA();
      if (currentmA > NUM_MODULES * 250) {
        disableAll("Over current");
      } else if (all_stopped && iterationStartMillis - stopped_at_millis > 100 && currentmA >= 3) {
        disableAll("Unexpected current");
      }
      lastCurrentReadMillis = iterationStartMillis;
    }
#endif

    if (all_idle) {

#if INA219_POWER_SENSE
        float voltage = powerSense.getBusVoltage_V();
        if (voltage > 14) {
          disableAll("Over voltage");
        } else if (voltage < 10) {
          disableAll("Under voltage");
        }
        // dtostrf(voltage, 6, 3, voltageBuf);
        // snprintf(displayBuffer, sizeof(displayBuffer), "%sV", voltageBuf);
        // display.setCursor(84, 0);
        // display.print(displayBuffer);

        // dtostrf(currentmA / 1000, 6, 3, currentBuf);
        // snprintf(displayBuffer, sizeof(displayBuffer), "%sA", currentBuf);
        // display.setCursor(84, 8);
        // display.print(displayBuffer);
#endif

      if (pending_no_op && all_stopped) {
        Serial.print(FAVR("{\"type\":\"no_op\"}\n"));
        Serial.flush();
        pending_no_op = false;
      }
      if (pending_move_response && all_stopped) {
        pending_move_response = false;
        dump_status();
      }

      while (Serial.available() > 0) {
        int b = Serial.read();
        switch (b) {
          case '@':
            for (uint8_t i = 0; i < NUM_MODULES; i++) {
              modules[i]->ResetErrorCounters();
if (i == 0) { // FIXME
              modules[i]->GoHome();
}
            }
            break;
          case '#':
            pending_no_op = true;
            break;
          case '=':
            recv_count = 0;
            break;
          case '\n':
              pending_move_response = true;
              Serial.print(FAVR("{\"type\":\"move_echo\", \"dest\":\""));
              Serial.flush();
              for (uint8_t i = 0; i < recv_count; i++) {
                int8_t index = FindFlapIndex(recv_buffer[i]);
                if (index != -1) {
                  if (FORCE_FULL_ROTATION || index != modules[i]->GetTargetFlapIndex()) {
                    modules[i]->GoToFlapIndex(index);
                  }
                }
                Serial.write(recv_buffer[i]);
                if (i % 8 == 0) {
                  Serial.flush();
                }
              }
              Serial.print(FAVR("\"}\n"));
              Serial.flush();
              break;
          default:
            if (recv_count > NUM_MODULES - 1) {
              break;
            }
            recv_buffer[recv_count] = b;
            recv_count++;
            break;
        }
      }
    }
}

void sensor_test_iteration() {
    motor_sensor_io();

    Serial.println();
    for (uint8_t i = 0; i < NUM_MODULES; i++) {
      Serial.print(modules[i]->GetHomeState() ? '0' : '1');
    }
    delay(100);
}


void loop() {
  while (1) {
#if SENSOR_TEST
    sensor_test_iteration();
#else
    run_iteration();
#endif

    // Yield to avoid triggering Soft WDT
    yield();
  }
}

void dump_status() {
  Serial.print(FAVR("{\"type\":\"status\", \"modules\":["));
  for (uint8_t i = 0; i < NUM_MODULES; i++) {
    Serial.print(FAVR("{\"state\":\""));
    switch (modules[i]->state) {
      case NORMAL:
        Serial.print(FAVR("normal"));
        break;
      case LOOK_FOR_HOME:
        Serial.print(FAVR("look_for_home"));
        break;
      case SENSOR_ERROR:
        Serial.print(FAVR("sensor_error"));
        break;
      case PANIC:
        Serial.print(FAVR("panic"));
        break;
      case STATE_DISABLED:
        Serial.print(FAVR("disabled"));
        break;
    }
    Serial.print(FAVR("\", \"flap\":\""));
    Serial.write(flaps[modules[i]->GetCurrentFlapIndex()]);
    Serial.print(FAVR("\", \"count_missed_home\":"));
    Serial.print(modules[i]->count_missed_home);
    Serial.print(FAVR(", \"count_unexpected_home\":"));
    Serial.print(modules[i]->count_unexpected_home);
    Serial.print(FAVR("}"));
    if (i < NUM_MODULES - 1) {
      Serial.print(FAVR(", "));
    }
  }
  Serial.print(FAVR("]}\n"));
  Serial.flush();
}
