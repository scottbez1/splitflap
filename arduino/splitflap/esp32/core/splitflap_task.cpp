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

// General splitflap includes
#include "config.h"
#include "src/splitflap_module.h"
#include "src/spi_io_config.h"

// ESP32-specific includes
#include "semaphore_guard.h"
#include "task.h"
#include "splitflap_task.h"

static_assert(QCMD_FLAP + NUM_FLAPS <= 255, "Too many flaps to fit in uint8_t command structure");

SplitflapTask::SplitflapTask(const uint8_t task_core, const LedMode led_mode) : Task("Splitflap", 8192, 1, task_core), led_mode_(led_mode), state_semaphore_(xSemaphoreCreateMutex()) {
  assert(state_semaphore_ != NULL);
  xSemaphoreGive(state_semaphore_);

  queue_ = xQueueCreate(5, sizeof(Command));
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

#if (defined(CHAINLINK) && !defined(CHAINLINK_DRIVER_TESTER))
#if CHAINLINK_ENFORCE_LOOPBACKS
    bool loopback_result[NUM_LOOPBACKS][NUM_LOOPBACKS];
    bool loopback_off_result[NUM_LOOPBACKS];
    bool loopback_success = chainlink_test_all_loopbacks(loopback_result, loopback_off_result);

    if (!loopback_success) {
      for (uint8_t i = 0; i < NUM_LOOPBACKS; i++) {
        for (uint8_t j = 0; j < NUM_LOOPBACKS; j++) {
          if (!loopback_result[i][j]) {
            char buffer[200] = {};
            snprintf(buffer, sizeof(buffer), "Bad loopback. Set output %u but read incorrect value at input %u", i, j);
            log(buffer);
          }
        }
      }
      for (uint8_t j = 0; j < NUM_LOOPBACKS; j++) {
        if (!loopback_off_result[j]) {
            char buffer[200] = {};
            snprintf(buffer, sizeof(buffer), "Bad loopback at input %u when all outputs off - should have been 0", j);
            log(buffer);
        }
      }

      while(1) {
          ESP_ERROR_CHECK(esp_task_wdt_reset());
      }
    }
#endif

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
#if !defined(CHAINLINK_DRIVER_TESTER) && !defined(CHAINLINK_BASE)
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
        switch (queue_receive_buffer_.command_type) {
            case CommandType::MODULES: {
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
                break;
            }
            case CommandType::SENSOR_TEST_SET:
                log("FOO");
                sensor_test_ = true;
                break;
            case CommandType::SENSOR_TEST_CLEAR:
                log("BAR");
                sensor_test_ = false;
                break;
        }
    }
}

void SplitflapTask::runUpdate() {
    boolean all_idle = true;

    uint32_t iterationStartMillis = millis();

    uint32_t flashStep = iterationStartMillis / 200;
    uint32_t flashGroup = (flashStep % 16) / 2;
    uint8_t flashPhase = flashStep % 2;

    if (sensor_test_ && all_stopped_) {
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
    } else {
      all_stopped_ = true;
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
        all_stopped_ &= is_stopped;
      }
      motor_sensor_io();
    }


#if defined(CHAINLINK) && CHAINLINK_ENFORCE_LOOPBACKS
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
        log("Loopback status changed to 0!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      }
    } else if (loopback_step_index_ == 50) {
      loopback_step_index_ = 0;
      loopback_current_out_index_ += 1;

      // If we've iterated through all loopbacks, save the results of this run and restart
      // from the first loopback again.
      if (loopback_current_out_index_ >= NUM_LOOPBACKS) {
        if (loopback_all_ok_ != loopback_current_ok_) {
            char buffer[200] = {};
            snprintf(buffer, sizeof(buffer), "Loopback status changed to %u", loopback_current_ok_);
            log(buffer);
        }
        loopback_all_ok_ = loopback_current_ok_;
        loopback_current_ok_ = true;
        loopback_current_out_index_ = 0;
      }
    }
    // TODO: handle loopback failures
#endif

    updateStateCache();
}

int8_t SplitflapTask::findFlapIndex(uint8_t character) {
    for (int8_t i = 0; i < NUM_FLAPS; i++) {
        if (character == flaps[i]) {
          return i;
        }
    }
    return -1;
}

void SplitflapTask::updateStateCache() {
    SplitflapState new_state;
    new_state.mode = sensor_test_ ? SplitflapMode::MODE_SENSOR_TEST : SplitflapMode::MODE_RUN;
    for (uint8_t i = 0; i < NUM_MODULES; i++) {
      new_state.modules[i].flap_index = modules[i]->GetCurrentFlapIndex();
      new_state.modules[i].state = modules[i]->state;
      new_state.modules[i].moving = modules[i]->current_accel_step > 0;
      new_state.modules[i].home_state = modules[i]->GetHomeState();
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

void SplitflapTask::log(const char* msg) {
    if (logger_ != nullptr) {
        logger_->log(msg);
    }
}

void SplitflapTask::showString(const char* str, uint8_t length) {
    Command command = {};
    command.command_type = CommandType::MODULES;
    for (uint8_t i = 0; i < length; i++) {
        int8_t index = findFlapIndex(str[i]);
        if (index != -1) {
            if (FORCE_FULL_ROTATION || index != modules[i]->GetTargetFlapIndex()) {
                command.data[i] = QCMD_FLAP + index;
            }
        }
    }
    assert(xQueueSendToBack(queue_, &command, portMAX_DELAY) == pdTRUE);
}

void SplitflapTask::resetAll() {
    Command command = {};
    command.command_type = CommandType::MODULES;
    for (uint8_t i = 0; i < NUM_MODULES; i++) {
        command.data[i] = QCMD_RESET_AND_HOME;
    }
    assert(xQueueSendToBack(queue_, &command, portMAX_DELAY) == pdTRUE);
}

void SplitflapTask::setLed(const uint8_t id, const bool on) {
    assert(led_mode_ == LedMode::MANUAL);

    Command command = {};
    command.command_type = CommandType::MODULES;
    command.data[id] = on ? QCMD_LED_ON : QCMD_LED_OFF;
    assert(xQueueSendToBack(queue_, &command, portMAX_DELAY) == pdTRUE);
}

void SplitflapTask::setSensorTest(bool sensor_test) {
    Command command = {};
    command.command_type = sensor_test ? CommandType::SENSOR_TEST_SET : CommandType::SENSOR_TEST_CLEAR;
    assert(xQueueSendToBack(queue_, &command, portMAX_DELAY) == pdTRUE);
}

SplitflapState SplitflapTask::getState() {
    SemaphoreGuard lock(state_semaphore_);
    return state_cache_;
}

void SplitflapTask::setLogger(Logger* logger) {
    logger_ = logger;
}

void SplitflapTask::postRawCommand(Command command) {
    assert(xQueueSendToBack(queue_, &command, portMAX_DELAY) == pdTRUE);
}
