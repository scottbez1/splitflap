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

#define QCMD_NO_OP          0
#define QCMD_RESET_AND_HOME 1
#define QCMD_LED_ON         2
#define QCMD_LED_OFF        3
#define QCMD_FLAP           4

static_assert(QCMD_FLAP + NUM_FLAPS <= 255, "Too many flaps to fit in uint8_t command structure");

SplitflapTask::SplitflapTask(const uint8_t task_core, const LedMode led_mode) : Task("Splitflap", 8192, 1, task_core), led_mode_(led_mode), state_semaphore_(xSemaphoreCreateMutex()) {
  assert(state_semaphore_ != NULL);
  xSemaphoreGive(state_semaphore_);

  queue_ = xQueueCreate( 1, sizeof(Command) );
  assert(queue_ != NULL);
}

SplitflapTask::~SplitflapTask() {
  if (queue_ != NULL) {
    vQueueDelete(queue_);
  }
  if (state_semaphore_ != NULL) {
    vSemaphoreDelete(state_semaphore_);
  }
}


void SplitflapTask::run() {
    esp_err_t result = esp_task_wdt_add(NULL);
    ESP_ERROR_CHECK(result);

    initialize_modules();

    // Initialize shift registers before turning on shift register output-enable
    motor_sensor_io();

#ifdef OUTPUT_ENABLE_PIN
    pinMode(OUTPUT_ENABLE_PIN, OUTPUT);
    digitalWrite(OUTPUT_ENABLE_PIN, LOW);
#endif

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
    bool loopback_success = chainlink_test_all_loopbacks(loopback_result, loopback_off_result);

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

      while(1) {
          ESP_ERROR_CHECK(esp_task_wdt_reset());
      }
    }

    if (led_mode_ == LedMode::AUTO) {
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

    for (uint8_t i = 0; i < NUM_MODULES; i++) {
        modules[i]->Init();
#ifndef CHAINLINK_DRIVER_TESTER
        modules[i]->GoHome();
#endif
    }

    while(1) {
        processQueue();
        runUpdate();
        result = esp_task_wdt_reset();
        ESP_ERROR_CHECK(result);
    }
}

void SplitflapTask::processQueue() {
    if (xQueueReceive(queue_, &queue_receive_buffer_, 0) == pdTRUE) {
        bool any_leds = false;
        for (uint8_t i = 0; i < NUM_MODULES; i++) {
            switch (queue_receive_buffer_.data[i]) {
                case QCMD_NO_OP:
                    // No-op
                    break;
                case QCMD_RESET_AND_HOME:
                    modules[i]->ResetState();
                    modules[i]->GoHome();
                    break;
                case QCMD_LED_ON:
                    any_leds = true;
#ifdef CHAINLINK
                    chainlink_set_led(i, true);
#endif
                    break;
                case QCMD_LED_OFF:
                    any_leds = true;
#ifdef CHAINLINK
                    chainlink_set_led(i, false);
#endif
                    break;
                default:
                    assert(queue_receive_buffer_.data[i] >= QCMD_FLAP && queue_receive_buffer_.data[i] < QCMD_FLAP + NUM_FLAPS);
                    modules[i]->GoToFlapIndex(queue_receive_buffer_.data[i] - QCMD_FLAP);
                    break;
            }
        }
        if (any_leds) {
            motor_sensor_io();
        }
    }
}

void SplitflapTask::runUpdate() {
    boolean all_idle = true;
    boolean all_stopped = true;

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

#ifdef CHAINLINK
        if (led_mode_ == LedMode::AUTO) {
          chainlink_set_led(i, flashGroup < modules[i]->state && flashPhase == 0);
        }
#endif

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

#ifdef CHAINLINK
      if (led_mode_ == LedMode::AUTO) {
        for (uint8_t i = 0; i < NUM_MODULES; i++) {
          chainlink_set_led(i, modules[i]->GetHomeState());
        }
        // Output LED state
        motor_sensor_io();
      }
#endif

      if (iterationStartMillis - last_sensor_print_millis_ > 200) {
        last_sensor_print_millis_ = iterationStartMillis;
        for (uint8_t i = 0; i < NUM_MODULES; i++) {
            Serial.write(modules[i]->GetHomeState() ? '1' : '0');
        }
        Serial.println();
      }
    }


#ifdef CHAINLINK
    // We test loopbacks iteratively, so as not to waste too many cycles/IO-roundtrips all at once. There are
    // two levels of iteration - loopback_step_index_ tracks the small intermediate steps of testing a single
    // loopback, and loopback_current_out_index_ tracks which loopback we're currently testing.
    loopback_step_index_++;
    if (loopback_step_index_ == 1) {
      chainlink_set_loopback(loopback_current_out_index_);
    } else if (loopback_step_index_ == 3) {
      bool ok = chainlink_validate_loopback(loopback_current_out_index_, nullptr);
      loopback_current_ok_ &= ok;

      if (!ok && loopback_all_ok_) {
        // Publish failures immediately
        loopback_all_ok_ = false;
        Serial.println("Loopback status changed to 0");
      }
    } else if (loopback_step_index_ == 50) {
      loopback_step_index_ = 0;
      loopback_current_out_index_ += 1;

      // If we've iterated through all loopbacks, save the results of this run and restart
      // from the first loopback again.
      if (loopback_current_out_index_ >= NUM_LOOPBACKS) {
        if (loopback_all_ok_ != loopback_current_ok_) {
          Serial.println("Loopback status changed to " + String((int)loopback_current_ok_));
        }
        loopback_all_ok_ = loopback_current_ok_;
        loopback_current_ok_ = true;
        loopback_current_out_index_ = 0;
      }
    }
    // TODO: handle loopback failures
#endif

    updateStateCache();

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
  // TODO: migrate to use state cache
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
    SplitflapState new_state;
    for (uint8_t i = 0; i < NUM_MODULES; i++) {
      new_state.modules[i].flap_index = modules[i]->GetCurrentFlapIndex();
      new_state.modules[i].state = modules[i]->state;
      new_state.modules[i].moving = modules[i]->current_accel_step > 0;
      new_state.modules[i].count_missed_home = modules[i]->count_missed_home;
      new_state.modules[i].count_unexpected_home = modules[i]->count_unexpected_home;
    }

#ifdef CHAINLINK
    new_state.loopbacks_ok = loopback_all_ok_;
#endif
    if (memcmp(&state_cache_, &new_state, sizeof(state_cache_))) {
        SemaphoreGuard lock(state_semaphore_);
        memcpy(&state_cache_, &new_state, sizeof(state_cache_));
    }
}

void SplitflapTask::showString(const char* str, uint8_t length) {
    // XXX This is very dangerous to call from within our own task, as we risk deadlock, but allow it for now to ease development. It will be safe once serial processing is moved to another task
    // assert(xTaskGetCurrentTaskHandle() != getHandle());

    pending_move_response = true;
    Serial.print("{\"type\":\"move_echo\", \"dest\":\"");
    Serial.flush();

    Command command = {};
    for (uint8_t i = 0; i < length; i++) {
        Serial.write(str[i]);
        int8_t index = findFlapIndex(str[i]);
        if (index != -1) {
            if (FORCE_FULL_ROTATION || index != modules[i]->GetTargetFlapIndex()) {
                command.data[i] = QCMD_FLAP + index;
            }
        }
    }

    assert(xQueueSendToBack(queue_, &command, portMAX_DELAY) == pdTRUE);

    Serial.print("\"}\n");
    Serial.flush();
}

void SplitflapTask::resetAll() {
    Command command = {};
    for (uint8_t i = 0; i < NUM_MODULES; i++) {
        command.data[i] = QCMD_RESET_AND_HOME;
    }
    assert(xQueueSendToBack(queue_, &command, portMAX_DELAY) == pdTRUE);
}

void SplitflapTask::setLed(const uint8_t id, const bool on) {
    assert(xTaskGetCurrentTaskHandle() != getHandle());
    assert(led_mode_ == LedMode::MANUAL);

    Command command = {};
    command.data[id] = on ? QCMD_LED_ON : QCMD_LED_OFF;
    assert(xQueueSendToBack(queue_, &command, portMAX_DELAY) == pdTRUE);
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
