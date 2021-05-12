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

#include <esp_task_wdt.h>

// General splitflap includes
#include "config.h"
#include "src/splitflap_module.h"
#include "src/spi_io_config.h"

// ESP32-specific includes
#include "semaphore_guard.h"
#include "task.h"
#include "splitflap_task.h"

SplitflapTask::SplitflapTask(const uint8_t task_core, const LedMode led_mode) : Task("Splitflap", 8192, 1, task_core), led_mode_(led_mode), module_semaphore_(xSemaphoreCreateMutex()), state_semaphore_(xSemaphoreCreateMutex()) {
  assert(module_semaphore_ != NULL);
  xSemaphoreGive(module_semaphore_);

  assert(state_semaphore_ != NULL);
  xSemaphoreGive(state_semaphore_);
}

SplitflapTask::~SplitflapTask() {
  if (module_semaphore_ != NULL) {
    vSemaphoreDelete(module_semaphore_);
  }
  if (state_semaphore_ != NULL) {
    vSemaphoreDelete(state_semaphore_);
  }
}


void SplitflapTask::run() {
    esp_err_t result = esp_task_wdt_add(NULL);
    ESP_ERROR_CHECK(result);

    {
        SemaphoreGuard lock(module_semaphore_);
        initialize_modules();

        // Initialize shift registers before turning on shift register output-enable
        motor_sensor_io();
    }

#ifdef OUTPUT_ENABLE_PIN
    pinMode(OUTPUT_ENABLE_PIN, OUTPUT);
    digitalWrite(OUTPUT_ENABLE_PIN, LOW);
#endif

    // XXX FIXME
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);

    // TODO: Move to serial task
    for (uint8_t i = 0; i < NUM_MODULES; i++) {
      recv_buffer[i] = 0;
    }
    Serial.print("\n\n\n");
    Serial.print("{\"type\":\"init\", \"num_modules\":");
    Serial.print(NUM_MODULES);
    Serial.print("}\n");

#if (defined(CHAINLINK) && !defined(CHAINLINK_DRIVER_TESTER))
    bool loopback_result[NUM_LOOPBACKS][NUM_LOOPBACKS];
    bool loopback_off_result[NUM_LOOPBACKS];
    bool loopback_success = testAllLoopbacks(loopback_result, loopback_off_result);

    if (!loopback_success) {
      for (uint8_t i = 0; i < NUM_LOOPBACKS; i++) {
        for (uint8_t j = 0; j < NUM_LOOPBACKS; j++) {
          if (!loopback_result[i][j]) {
            Serial.printf("Bad loopback. Set output %u but read incorrect value at input %u\n", i, j);
          }
        }
      }
      for (uint8_t j = 0; j < NUM_LOOPBACKS; j++) {
        if (!loopback_off_result[j]) {
          Serial.printf("Bad loopback at input %u when all outputs off - should have been 0\n", j);
        }
      }

      // XXX FIXME
      // // Loop forever
      // while(1) {
      //     ESP_ERROR_CHECK(esp_task_wdt_reset());
      // }
    }

    if (led_mode_ == LedMode::AUTO) {
      SemaphoreGuard lock(module_semaphore_);
      for (uint8_t r = 0; r < 3; r++) {
        for (uint8_t i = 0; i < NUM_MODULES; i++) {
          chainlink_set_led(i, 1);
          motor_sensor_io();
          delay(10);
          chainlink_set_led(i, 0);
          motor_sensor_io();
        }
        result = esp_task_wdt_reset();
        ESP_ERROR_CHECK(result);
        delay(500);
      }
    }
#endif

    {
        SemaphoreGuard lock(module_semaphore_);
        for (uint8_t i = 0; i < NUM_MODULES; i++) {
            modules[i]->Init();
#ifndef CHAINLINK_DRIVER_TESTER
            modules[i]->GoHome();
#endif
        }
    }

    while(1) {
        runUpdate();
        result = esp_task_wdt_reset();
        ESP_ERROR_CHECK(result);
    }
}

bool SplitflapTask::testAllLoopbacks(bool loopback_result[NUM_LOOPBACKS][NUM_LOOPBACKS], bool loopback_off_result[NUM_LOOPBACKS]) {
    SemaphoreGuard lock(module_semaphore_);
    bool loopback_success = true;

    // Turn one loopback bit on at a time and make sure only that loopback bit is set
    for (uint8_t loop_out_index = 0; loop_out_index < NUM_LOOPBACKS; loop_out_index++) {
      loopback_success &= chainlink_test_loopback(loop_out_index, loopback_result[loop_out_index]);
    }

    loopback_success &= chainlink_test_startup_loopback(loopback_off_result);

    return loopback_success;
}

void SplitflapTask::setLed(const uint8_t id, const bool on) {
    assert(led_mode_ == LedMode::MANUAL);

    SemaphoreGuard lock(module_semaphore_);
    chainlink_set_led(id, on);
    motor_sensor_io();
}

void SplitflapTask::runUpdate() {
    boolean all_idle = true;
    boolean all_stopped = true;

    {
      SemaphoreGuard lock(module_semaphore_);
      uint32_t iterationStartMillis = millis();

      uint32_t flashStep = iterationStartMillis / 200;
      uint32_t flashGroup = (flashStep % 16) / 2;
      uint8_t flashPhase = flashStep % 2;

      if (!sensor_test_) {
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

          if (led_mode_ == LedMode::AUTO) {
            chainlink_set_led(i, flashGroup < modules[i]->state && flashPhase == 0);
          }

          all_idle &= is_idle;
          all_stopped &= is_stopped;
        }
        if (all_stopped && !was_stopped) {
          stopped_at_millis = iterationStartMillis;
        }
        was_stopped = all_stopped;
        motor_sensor_io();
      } else {
        // Read sensor state
        motor_sensor_io();

        if (led_mode_ == LedMode::AUTO) {
          for (uint8_t i = 0; i < NUM_MODULES; i++) {
            chainlink_set_led(i, modules[i]->GetHomeState());
          }
          // Output LED state
          motor_sensor_io();
        }

        if (iterationStartMillis - last_sensor_print_millis_ > 200) {
          last_sensor_print_millis_ = iterationStartMillis;
          for (uint8_t i = 0; i < NUM_MODULES; i++) {
              Serial.write(modules[i]->GetHomeState() ? '1' : '0');
          }
          Serial.println();
        }
      }
      updateStateCache();
    }
    taskYIELD();

    if (all_idle) {
      if (pending_no_op && all_stopped) {
        Serial.print("{\"type\":\"no_op\"}\n");
        Serial.flush();
        pending_no_op = false;
      }
      if (pending_move_response && all_stopped) {
        pending_move_response = false;
        dumpStatus();
      }

      while (Serial.available() > 0) {
        int b = Serial.read();
        if (b == '%' && all_stopped) {
          sensor_test_ = !sensor_test_;
          Serial.print("{\"type\":\"sensor_test\", \"enabled\":");
          Serial.print(sensor_test_ ? "true" : "false");
          Serial.print("}\n");
        } else if (sensor_test_ == false) {
          switch (b) {
            case '@':
              resetAll();
              break;
            case '#':
              pending_no_op = true;
              break;
            case '=':
              recv_count = 0;
              break;
            case '\n':
                showString(recv_buffer, recv_count);
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
}

int8_t SplitflapTask::findFlapIndex(uint8_t character) {
    for (int8_t i = 0; i < NUM_FLAPS; i++) {
        if (character == flaps[i]) {
          return i;
        }
    }
    return -1;
}

void SplitflapTask::dumpStatus() {
  // TODO: migrate to use state cache instead of modules w/ locking
  SemaphoreGuard lock(module_semaphore_);

  Serial.print("{\"type\":\"status\", \"modules\":[");
  for (uint8_t i = 0; i < NUM_MODULES; i++) {
    Serial.print("{\"state\":\"");
    switch (modules[i]->state) {
      case NORMAL:
        Serial.print("normal");
        break;
      case LOOK_FOR_HOME:
        Serial.print("look_for_home");
        break;
      case SENSOR_ERROR:
        Serial.print("sensor_error");
        break;
      case PANIC:
        Serial.print("panic");
        break;
      case STATE_DISABLED:
        Serial.print("disabled");
        break;
    }
    Serial.print("\", \"flap\":\"");
    Serial.write(flaps[modules[i]->GetCurrentFlapIndex()]);
    Serial.print("\", \"count_missed_home\":");
    Serial.print(modules[i]->count_missed_home);
    Serial.print(", \"count_unexpected_home\":");
    Serial.print(modules[i]->count_unexpected_home);
    Serial.print("}");
    if (i < NUM_MODULES - 1) {
      Serial.print(", ");
    }
  }
  Serial.print("]}\n");
  Serial.flush();
}

void SplitflapTask::updateStateCache() {
    // Must already be holding module mutex when updating the state cache
    assert(xSemaphoreGetMutexHolder(module_semaphore_) == getHandle());

    SplitflapState new_state;
    for (uint8_t i = 0; i < NUM_MODULES; i++) {
      new_state.modules[i].flapIndex = modules[i]->GetCurrentFlapIndex();
      new_state.modules[i].state = modules[i]->state;
      new_state.modules[i].moving = modules[i]->current_accel_step > 0;
    }
    if (memcmp(&state_cache_, &new_state, sizeof(state_cache_))) {
        SemaphoreGuard lock(state_semaphore_);
        memcpy(&state_cache_, &new_state, sizeof(state_cache_));
    }
}

void SplitflapTask::showString(const char* str, uint8_t length) {
    SemaphoreGuard lock(module_semaphore_);
    pending_move_response = true;
    Serial.print("{\"type\":\"move_echo\", \"dest\":\"");
    Serial.flush();
    for (uint8_t i = 0; i < length; i++) {
        int8_t index = findFlapIndex(str[i]);
        if (index != -1) {
            if (FORCE_FULL_ROTATION || index != modules[i]->GetTargetFlapIndex()) {
            modules[i]->GoToFlapIndex(index);
            }
        }
        Serial.write(str[i]);
        if (i % 8 == 0) {
            Serial.flush();
        }
    }
    Serial.print("\"}\n");
    Serial.flush();
}

void SplitflapTask::resetAll() {
    SemaphoreGuard lock(module_semaphore_);
    for (uint8_t i = 0; i < NUM_MODULES; i++) {
        modules[i]->ResetErrorCounters();
        modules[i]->GoHome();
    }
}

SplitflapState SplitflapTask::getState() {
    SemaphoreGuard lock(state_semaphore_);
    return state_cache_;
}

// void SplitflapTask::disableAll(const char* message) {
//   for (uint8_t i = 0; i < NUM_MODULES; i++) {
//     modules[i]->Disable();
//   }
//   motor_sensor_io();

//   if (disabled) {
//     return;
//   }
//   disabled = true;

//   Serial.println("#### DISABLED! ####");
//   Serial.println(message);
// }
