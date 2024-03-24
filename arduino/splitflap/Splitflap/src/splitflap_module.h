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

#ifndef SPLITFLAP_MODULE_H
#define SPLITFLAP_MODULE_H

#include <Arduino.h>

#include "acceleration.h"
#include "splitflap_module_data.h"
#include "../config.h"

// Logging and assertions are useful for debugging, but likely add too much time/space overhead to be used when
// driving many SplitflapModules at once.
#define VERBOSE_LOGGING false
#define ASSERTIONS_ENABLED false

#define FAKE_HOME_SENSOR false

// Must be an integer. This code does not support a fractional steps per revolution for motors with non-integral gear ratios
#define STEPS_PER_REVOLUTION (2048)

// This is "rough" because it's integer division; it shouldn't be used for movement calculations or the error would
// accumulate.
#define _ROUGH_STEPS_PER_FLAP (STEPS_PER_REVOLUTION / NUM_FLAPS)

#if HOME_CALIBRATION_ENABLED
// The number of steps in either direction that's acceptable error for the home sensor
#define HOME_ERROR_MARGIN_STEPS (_ROUGH_STEPS_PER_FLAP / 4)

// After finding the home position, how long to wait before considering another home blip to be an unexpected error
#define UNEXPECTED_HOME_START_BUFFER_STEPS (_ROUGH_STEPS_PER_FLAP * 5)

// When recalibrating the home position, the number of steps to travel searching for home before giving up
#define MAX_STEPS_LOOKING_FOR_HOME ((NUM_FLAPS + 2) * _ROUGH_STEPS_PER_FLAP)

#define UNEXPECTED_HOME_START_STEP UNEXPECTED_HOME_START_BUFFER_STEPS  // Start of range where a home sensor blip is unexpected
#define UNEXPECTED_HOME_END_STEP (STEPS_PER_REVOLUTION - HOME_ERROR_MARGIN_STEPS)  // End of range where a home sensor blip is unexpected

// Expected home position step plus some margin of error. If we get to this step without having seen a home
// sensor blip, something is wrong and we need to recalibrate.
#define MISSED_HOME_STEP HOME_ERROR_MARGIN_STEPS
#endif

class SplitflapModule {
 private:
  // Configuration:
  uint8_t &motor_out;
  const uint8_t motor_bitshift;

  uint8_t &sensor_in;
  const uint8_t sensor_bitmask;

  // State:
  bool last_home = false;
  unsigned long last_update_micros = 0;

  // Tracks the most recent target flap index. Not used during motion, but needed to recalculate target step if we
  // re-calibrate the home position
  uint8_t target_flap_index = 0;

  // Current position/destination. Numbers are modulo GEAR_RATIO_INPUT_STEPS
  uint16_t current_step = 0;
  uint16_t delta_steps = 0;

  uint16_t offset_steps = 1969;

#if HOME_CALIBRATION_ENABLED
  // Home calibration state
  HomeState home_state = IGNORE;
#endif

  // Motor state
  uint8_t current_phase = 0;
  uint16_t current_period = Acceleration::ACCEL_STEP_PERIODS[0];

  void Panic(String message);
  bool CheckSensor();
  void SetMotor(uint8_t out);

  uint8_t GetFlapFloor(uint16_t step);
  uint16_t GetTargetStepForFlapIndex(uint8_t target_flap_index);
  void GoToTargetFlapIndex();
  void UpdateExpectedHome();

 public:
  SplitflapModule(
    uint8_t &motor_out,
    const uint8_t motor_bitshift,
    uint8_t &sensor_in,
    const uint8_t sensor_bitmask
  );

#if HOME_CALIBRATION_ENABLED
  State state = SENSOR_ERROR; // Start in SENSOR_ERROR state until initialized
#else
  State state = NORMAL;
#endif
  uint8_t current_accel_step = 0;

  void GoToFlapIndex(uint8_t index);
  uint8_t GetCurrentFlapIndex();
  uint8_t GetTargetFlapIndex();
  void FindAndRecalibrateHome();
  void ResetErrorCounters();
  void ResetState();
  inline void Update();
  void Init();
  bool GetHomeState();
  void Disable();
  
  uint8_t count_unexpected_home = 0;
  uint8_t count_missed_home = 0;
};



#define MOT_PHASE_A B00001000
#define MOT_PHASE_B B00000100
#define MOT_PHASE_C B00000010
#define MOT_PHASE_D B00000001

const uint8_t step_pattern[] = {
#if REVERSE_MOTOR_DIRECTION
  MOT_PHASE_D | MOT_PHASE_A,
  MOT_PHASE_C | MOT_PHASE_D,
  MOT_PHASE_B | MOT_PHASE_C,
  MOT_PHASE_A | MOT_PHASE_B,
#else
  MOT_PHASE_A | MOT_PHASE_B,
  MOT_PHASE_B | MOT_PHASE_C,
  MOT_PHASE_C | MOT_PHASE_D,
  MOT_PHASE_D | MOT_PHASE_A,
#endif
};

SplitflapModule::SplitflapModule(
  uint8_t &motor_out,
  const uint8_t motor_bitshift,
  uint8_t &sensor_in,
  const uint8_t sensor_bitmask) :
    motor_out(motor_out),
    motor_bitshift(motor_bitshift),
    sensor_in(sensor_in),
    sensor_bitmask(sensor_bitmask)
{
}

void SplitflapModule::Disable() {
  SetMotor(0);
  state = STATE_DISABLED;
}

void SplitflapModule::Panic(String message) {
  SetMotor(0);
  state = PANIC;
  Serial.print("#### PANIC! ####\n");
  Serial.print(message);
}

__attribute__((always_inline))
inline bool SplitflapModule::CheckSensor() {
    bool cur_home = (sensor_in & sensor_bitmask) != 0;
    bool shift = cur_home == true && last_home == false;
    last_home = cur_home;

    return shift;
}

__attribute__((always_inline))
inline void SplitflapModule::SetMotor(uint8_t out) {
  motor_out = (motor_out & ~(0x0F << motor_bitshift)) | ((out & 0x0F) << motor_bitshift);
}

__attribute__((always_inline))
inline uint8_t SplitflapModule::GetFlapFloor(uint16_t step) {
    uint16_t step_without_offset = step >= offset_steps ? step - offset_steps : STEPS_PER_REVOLUTION + step - offset_steps;
    return (uint32_t)step_without_offset * NUM_FLAPS / STEPS_PER_REVOLUTION;
}

__attribute__((always_inline))
inline uint16_t SplitflapModule::GetTargetStepForFlapIndex(uint8_t flap) {
    uint32_t intermediate = (uint32_t)flap * STEPS_PER_REVOLUTION;

    // Round UP when dividing so that the inverse calculation on the result (GetFlapFloor) returns the expected
    // result.
    uint16_t result = intermediate / NUM_FLAPS;
    if (result % NUM_FLAPS != 0) {
        result++;
    }
    result += offset_steps;
    if (result >= STEPS_PER_REVOLUTION) {
        result -= STEPS_PER_REVOLUTION;
    }
    return result;
}

__attribute__((always_inline))
inline void SplitflapModule::GoToTargetFlapIndex() {
    if (state != NORMAL) {
        return;
    }
    uint16_t target_step = GetTargetStepForFlapIndex(target_flap_index);

    uint16_t minimum_stopping_step = current_step + current_accel_step; // Can't come to a stop until we've used up any remaining deceleration steps

    if (target_step <= minimum_stopping_step) {
        // Must go around
        delta_steps = STEPS_PER_REVOLUTION - current_step + target_step;
        // NB: delta_steps can be > 1 full revolution in the event we are not yet at current_step, but couldn't stop in time
    } else {
        delta_steps = target_step - current_step;
    }
}

__attribute__((always_inline))
inline void SplitflapModule::GoToFlapIndex(uint8_t index) {
    if (state != NORMAL
#if HOME_CALIBRATION_ENABLED
     && state != LOOK_FOR_HOME
#endif
    ) {
        return;
    }
    target_flap_index = index;
    GoToTargetFlapIndex();
}

__attribute__((always_inline))
inline uint8_t SplitflapModule::GetCurrentFlapIndex() {
   return GetFlapFloor(current_step);
}

uint8_t SplitflapModule::GetTargetFlapIndex() {
   return target_flap_index;
}

__attribute__((always_inline))
inline void SplitflapModule::FindAndRecalibrateHome() {
#if HOME_CALIBRATION_ENABLED
    if (state == PANIC || state == STATE_DISABLED) {
        return;
    }

    state = LOOK_FOR_HOME;
    delta_steps = MAX_STEPS_LOOKING_FOR_HOME;
#endif
}

__attribute__((always_inline))
inline void SplitflapModule::Update() {
    if (state == PANIC || state == STATE_DISABLED) {
        return;
    }

    unsigned long now = micros();
    unsigned long delta_time = now - last_update_micros;
    if (delta_time >= current_period) {
        last_update_micros = now;

        uint8_t target_accel_step;

        if (state == NORMAL) {
            bool reset_to_home = false;
#if HOME_CALIBRATION_ENABLED
            bool found_home = CheckSensor();
            if (home_state == IGNORE) {
#if VERBOSE_LOGGING
                if (found_home) {
                    Serial.print("VERBOSE: Ignoring home");
                }
#endif
                if (current_step == UNEXPECTED_HOME_START_STEP) {
                    home_state = UNEXPECTED;
                }
            } else if (home_state == UNEXPECTED) {
                if (found_home) {
                  count_unexpected_home++;
#if VERBOSE_LOGGING
                    Serial.print("VERBOSE: Unexpected home! At ");
                    Serial.print(current_step);
                    Serial.print(". Unexpected range ");
                    Serial.print(UNEXPECTED_HOME_START_STEP);
                    Serial.print('-');
                    Serial.print(UNEXPECTED_HOME_END_STEP);
                    Serial.print("; missed at ");
                    Serial.print(MISSED_HOME_STEP);
                    Serial.print(".\n");
#endif
                    reset_to_home = true;
                } else if (current_step == UNEXPECTED_HOME_END_STEP) {
                    home_state = EXPECTED;
                }
            } else if (home_state == EXPECTED) {
                if (FAKE_HOME_SENSOR || found_home) {
#if VERBOSE_LOGGING
                    Serial.print("VERBOSE: Found expected home.");
#endif
                    home_state = IGNORE;
                } else if (current_step == MISSED_HOME_STEP) {
                  count_missed_home++;
#if VERBOSE_LOGGING
                    Serial.print("VERBOSE: Missed expected home! At ");
                    Serial.print(current_step);
                    Serial.print(". Expected between ");
                    Serial.print(UNEXPECTED_HOME_END_STEP);
                    Serial.print(" and ");
                    Serial.print(MISSED_HOME_STEP);
                    Serial.print(".\n");
#endif
                    reset_to_home = true;
                }
            }
#endif

            if (reset_to_home) {
                FindAndRecalibrateHome();
                target_accel_step = 0;
            } else {
                // Update speed based on distance to target
                if (delta_steps > Acceleration::MAX_ACCEL_STEP) {
                    target_accel_step = Acceleration::MAX_ACCEL_STEP;
                } else {
                    target_accel_step = delta_steps;
                }
            }
#if HOME_CALIBRATION_ENABLED
        } else if (state == LOOK_FOR_HOME) {
            bool found_home = CheckSensor();
            if (FAKE_HOME_SENSOR || found_home) {
#if VERBOSE_LOGGING
                Serial.print("VERBOSE: Found home!\n");
#endif
                state = NORMAL;
                target_accel_step = 0;

                // Reset frame of reference
                current_step = 0;
                home_state = IGNORE;

                GoToTargetFlapIndex();
            } else {
                if (delta_steps == 0) {
#if VERBOSE_LOGGING
                    Serial.print("VERBOSE: Gave up looking for home!\n");
#endif
                    state = SENSOR_ERROR;
                    target_accel_step = 0;
                } else {
                    target_accel_step = Acceleration::MAX_ACCEL_STEP / 8;
                }
            }
#endif
        } else {
            target_accel_step = 0;
        }

        // Update motor
        if (current_accel_step < target_accel_step) {
            current_accel_step++;
        } else if (current_accel_step > target_accel_step) {
            current_accel_step--;
        }

        current_period = pgm_read_word_near(Acceleration::ACCEL_STEP_PERIODS + current_accel_step);

        if (current_accel_step > 0) {
            current_step++;
            if (current_step == STEPS_PER_REVOLUTION) {
                current_step = 0;
            }
            current_phase++;
            if (current_phase == 4) {
                current_phase = 0;
            }
            if (delta_steps > 0) {
                delta_steps--;
            }
            SetMotor(step_pattern[current_phase]);
        } else {
            SetMotor(0);
        }

#if ASSERTIONS_ENABLED
        // Check modular arithmetic invariant
        if (current_step >= STEPS_PER_REVOLUTION) {
            Panic("current_step >= STEPS_PER_REVOLUTION");
        }
#endif
    }
    return;
}

void SplitflapModule::ResetErrorCounters() {
  count_unexpected_home = 0;
  count_missed_home = 0;
}

void SplitflapModule::ResetState() {
    ResetErrorCounters();
    CheckSensor();

    target_flap_index = 0;
    current_step = 0;
    delta_steps = 0;

#if HOME_CALIBRATION_ENABLED
    home_state = IGNORE;
#endif
}

void SplitflapModule::Init() {
    CheckSensor();
}

bool SplitflapModule::GetHomeState() {
  return (sensor_in & sensor_bitmask) != 0;
}

#endif
