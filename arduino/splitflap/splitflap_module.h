/*
   Copyright 2015-2018 Scott Bezek and the splitflap contributors

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

// Logging and assertions are useful for debugging, but likely add too much time/space overhead to be used when
// driving many SplitflapModules at once.
#define VERBOSE_LOGGING false
#define ASSERTIONS_ENABLED false

// Enable for auto-calibration via home sensor feedback. Disable for basic open-loop control (useful when first
// testing the split-flap, since home calibration can be tricky to fine tune)
#define HOME_CALIBRATION_ENABLED true

#define NUM_FLAPS (40)

#define STEPS_PER_MOTOR_REVOLUTION (32)

// The gear ratio constants below represent the input:output ratio of the gearbox expressed as a simplified fraction.
// For example, for a gear train with ratios 31:10, 26:9, 22:11, 32:9, the overall ratio expressed as integers would be
// (31*26*22*32):(10*9*11*9) == 567424:8910 == 25792:405 ~= 63.684:1. To avoid floating point math, we would use the
// simplified integer fraction values 25792 and 405.
#define _GEAR_RATIO_INPUT (128)
#define _GEAR_RATIO_OUTPUT (2)

// All motion is tracked in terms of motor steps (rather than motor revolutions), so we pre-multiply the gear ratio input
// by the number of motor steps per revolution as a more useful quantity to work with.
#define GEAR_RATIO_INPUT_STEPS (STEPS_PER_MOTOR_REVOLUTION * _GEAR_RATIO_INPUT)

// Likewise, we care about the number of flaps flipped, rather than the number of output shaft revolutions, so we
// pre-multiply the gear ratio output by the number of flaps per revolution as a more useful quantity to work with.
#define GEAR_RATIO_OUTPUT_FLAPS (_GEAR_RATIO_OUTPUT * NUM_FLAPS)

// This is "rough" because it's integer division; it shouldn't be used for movement calculations or the error would
// accumulate.
#define _ROUGH_STEPS_PER_FLAP (GEAR_RATIO_INPUT_STEPS / GEAR_RATIO_OUTPUT_FLAPS)

#if HOME_CALIBRATION_ENABLED
// The number of steps in either direction that's acceptable error for the home sensor
#define HOME_ERROR_MARGIN_STEPS (_ROUGH_STEPS_PER_FLAP / 4)

// After finding the home position, how long to wait before considering another home blip to be an unexpected error
#define UNEXPECTED_HOME_START_BUFFER_STEPS (_ROUGH_STEPS_PER_FLAP * 5)

// When recalibrating the home position, the number of steps to travel searching for home before giving up
#define MAX_STEPS_LOOKING_FOR_HOME ((NUM_FLAPS + 5) * _ROUGH_STEPS_PER_FLAP)

enum HomeState {
    // Ignore any home blips (e.g. if we've just seen the home position and haven't traveled past it yet)
    IGNORE,
    // Home isn't expected; a home blip in this state/region indicates an error that requires recalibration
    UNEXPECTED,
    // Home position is expected in this state/region
    EXPECTED,
};
#endif

enum State {
  NORMAL,
  PANIC,
#if HOME_CALIBRATION_ENABLED
  LOOK_FOR_HOME,
  SENSOR_ERROR,
#endif
};

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
  uint32_t current_step = 0;
  uint32_t delta_steps = 0;

#if HOME_CALIBRATION_ENABLED
  // Home calibration state. All values recalculated whenever we see a home sensor blip
  HomeState home_state = IGNORE;
  uint32_t unexpected_home_start_step = 0;  // Start of range where a home sensor blip is unexpected
  uint32_t unexpected_home_end_step = 0;  // End of range where a home sensor blip is unexpected

  // Expected home position step plus some margin of error. If we get to this step without having seen a home
  // sensor blip, something is wrong and we need to recalibrate.
  uint32_t missed_home_step = 0;
#endif

  // Motor state
  uint8_t current_phase = 0;
  uint16_t current_period = Acceleration::ACCEL_STEP_PERIODS[0];

  void Panic(String message);
  bool CheckSensor();
  void SetMotor(uint8_t out);

  uint8_t GetFlapFloor(uint32_t step);
  uint32_t GetTargetStepForFlapIndex(uint32_t from_step, uint8_t target_flap_index);
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
  State state = LOOK_FOR_HOME;
#else
  State state = NORMAL;
#endif
  uint8_t current_accel_step = 0;

  void GoToFlapIndex(uint8_t index);
  uint8_t GetCurrentFlapIndex();
  void GoHome();
  void ResetErrorCounters();
  inline bool Update();
  void Init();
  bool GetHomeState();
  
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

void SplitflapModule::Panic(String message) {
  Serial.print("#### PANIC! ####\n");
  Serial.print(message);
  SetMotor(0);
  state = PANIC;
}

__attribute__((always_inline))
bool SplitflapModule::CheckSensor() {
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
inline uint8_t SplitflapModule::GetFlapFloor(uint32_t step) {
    return step * GEAR_RATIO_OUTPUT_FLAPS / GEAR_RATIO_INPUT_STEPS;
}

__attribute__((always_inline))
inline uint32_t SplitflapModule::GetTargetStepForFlapIndex(uint32_t from_step, uint8_t target_flap_index) {

#if ASSERTIONS_ENABLED
    //assert 0 <= from_step < 2*GEAR_RATIO_INPUT_STEPS
    if (from_step < 0 || from_step >= 2 * GEAR_RATIO_INPUT_STEPS) {
        Panic("from_step < 0 || from_step >= 2 * GEAR_RATIO_INPUT_STEPS");
    }
#endif

    uint8_t from_flap = GetFlapFloor(from_step);

#if ASSERTIONS_ENABLED
    //assert 0 <= from_flap < 2*NUM_FLAPS
    if (from_flap < 0 || from_flap >= 2 * NUM_FLAPS) {
        Panic("from_flap < 0 || from_flap >= 2 * NUM_FLAPS");
    }
#endif

    uint8_t from_flap_index;
    if (from_flap >= NUM_FLAPS) {
        from_flap_index = from_flap - NUM_FLAPS;
    } else {
        from_flap_index = from_flap;
    }

    int8_t delta_flaps;
    if (target_flap_index > from_flap_index) {
        delta_flaps = target_flap_index - from_flap_index;
    } else {
        // Even if we're exactly at the target flap index, still do a full revolution to get to the target flap
        // since we're working with rounded numbers
        delta_flaps = NUM_FLAPS + target_flap_index - from_flap_index;
    }

#if VERBOSE_LOGGING
    Serial.print("VERBOSE: DELTA FLAPS=");
    Serial.print(delta_flaps);
    Serial.print('\n');
#endif

#if ASSERTIONS_ENABLED
    //assert 0 < delta_flaps <= 40
    if (delta_flaps <= 0 || delta_flaps > NUM_FLAPS) {
        Panic("delta_flaps <= 0 || delta_flaps > NUM_FLAPS");
    }
#endif

    uint32_t gear_ratio_input_steps_flaps_destination = ((uint32_t)from_flap + (uint32_t)delta_flaps) * GEAR_RATIO_INPUT_STEPS;

    // Round UP when dividing so that the inverse calculation on the result (_get_flap_floor) returns the expected
    // result.
    uint32_t result = gear_ratio_input_steps_flaps_destination / GEAR_RATIO_OUTPUT_FLAPS;
    if (gear_ratio_input_steps_flaps_destination % GEAR_RATIO_OUTPUT_FLAPS != 0) {
        result++;
    }
    return result;
}

__attribute__((always_inline))
inline void SplitflapModule::GoToTargetFlapIndex() {
    delta_steps = GetTargetStepForFlapIndex(current_step, target_flap_index) - current_step;


#if VERBOSE_LOGGING
    Serial.print("Going to flap index ");
    Serial.print(target_flap_index);
    Serial.print(". Current step is ");
    Serial.print(current_step);
    Serial.print(". Delta is ");
    Serial.print(delta_steps);
    Serial.print('\n');
#endif

#if ASSERTIONS_ENABLED
    if (delta_steps > GEAR_RATIO_INPUT_STEPS) {
        Panic("delta_steps > GEAR_RATIO_INPUT_STEPS");
    }
#endif
}

__attribute__((always_inline))
inline void SplitflapModule::UpdateExpectedHome() {
#if HOME_CALIBRATION_ENABLED
    // Expected home position is the next 0 index flap position after the missed_home_step. This must be calculated
    // from the missed_home_step, rather than current_step, so that in the event of an early home, we don't compute
    // the next home as the one that is just a few steps away.

    uint32_t expected_home = GetTargetStepForFlapIndex(missed_home_step, 0);

    uint32_t new_unexpected_home_start_step = current_step + UNEXPECTED_HOME_START_BUFFER_STEPS;
    uint32_t new_unexpected_home_end_step = expected_home - HOME_ERROR_MARGIN_STEPS;
    uint32_t new_missed_home_step = expected_home + HOME_ERROR_MARGIN_STEPS;

#if VERBOSE_LOGGING
    Serial.print("Calculated new expected home ");
    Serial.print(expected_home);
    Serial.print(".\nOLD:us=");
    Serial.print(unexpected_home_start_step);
    Serial.print(", ue=");
    Serial.print(unexpected_home_end_step);
    Serial.print(", m=");
    Serial.print(missed_home_step);
    Serial.print("\nNEW:us=");
    Serial.print(new_unexpected_home_start_step);
    Serial.print(", ue=");
    Serial.print(new_unexpected_home_end_step);
    Serial.print(", m=");
    Serial.print(new_missed_home_step);
    Serial.print('\n');
#endif

#if ASSERTIONS_ENABLED
    // Values shouldn't be more than 2*GEAR_RATIO_INPUT_STEPS, so use subtraction to bound to GEAR_RATIO_INPUT_STEPS
    // rather than using `%` which may be more expensive
    //assert 0 <= new_unexpected_home_start_step < 2*GEAR_RATIO_INPUT_STEPS
    if (new_unexpected_home_start_step >= 2 * GEAR_RATIO_INPUT_STEPS) {
        Panic("new_unexpected_home_start_step >= 2 * GEAR_RATIO_INPUT_STEPS");
    }
    //assert 0 <= new_unexpected_home_end_step < 2*GEAR_RATIO_INPUT_STEPS
    if (new_unexpected_home_end_step >= 2 * GEAR_RATIO_INPUT_STEPS) {
        Panic("new_unexpected_home_end_step >= 2 * GEAR_RATIO_INPUT_STEPS");
    }
    //assert 0 <= new_missed_home_step < 2*GEAR_RATIO_INPUT_STEPS
    if (new_missed_home_step >= 2 * GEAR_RATIO_INPUT_STEPS) {
        Panic("new_missed_home_step >= 2 * GEAR_RATIO_INPUT_STEPS");
    }
#endif

    if (new_unexpected_home_start_step >= GEAR_RATIO_INPUT_STEPS) {
        new_unexpected_home_start_step -= GEAR_RATIO_INPUT_STEPS;
    }
    if (new_unexpected_home_end_step >= GEAR_RATIO_INPUT_STEPS) {
        new_unexpected_home_end_step -= GEAR_RATIO_INPUT_STEPS;
    }
    if (new_missed_home_step >= GEAR_RATIO_INPUT_STEPS) {
        new_missed_home_step -= GEAR_RATIO_INPUT_STEPS;
    }

#if ASSERTIONS_ENABLED
    // The "unexpected" range should never wrap around, since GEAR_RATIO_INPUT_STEPS represents an integer number of
    // FULL revolutions.
    //assert new_unexpected_home_end_step > new_unexpected_home_start_step
    if (new_unexpected_home_end_step <= new_unexpected_home_start_step) {
        Panic("new_unexpected_home_end_step <= new_unexpected_home_start_step");
    }
#endif

    unexpected_home_start_step = new_unexpected_home_start_step;
    unexpected_home_end_step = new_unexpected_home_end_step;
    missed_home_step = new_missed_home_step;
    home_state = IGNORE;
#endif
}


__attribute__((always_inline))
inline void SplitflapModule::GoToFlapIndex(uint8_t index) {
    if (state != NORMAL || current_accel_step != 0) {
        return;
    }
    target_flap_index = index;
    GoToTargetFlapIndex();
}

__attribute__((always_inline))
inline uint8_t SplitflapModule::GetCurrentFlapIndex() {
   return (uint8_t)(GetFlapFloor(current_step) % NUM_FLAPS);
}

__attribute__((always_inline))
inline void SplitflapModule::GoHome() {
#if HOME_CALIBRATION_ENABLED
    if (state == PANIC) {
        return;
    }

    state = LOOK_FOR_HOME;
    delta_steps = MAX_STEPS_LOOKING_FOR_HOME;
#endif
}

__attribute__((always_inline))
inline bool SplitflapModule::Update() {
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
                if (found_home) {
#if VERBOSE_LOGGING
                    Serial.print("VERBOSE: Ignoring home");
#endif
                }
                if (current_step == unexpected_home_start_step) {
                    home_state = UNEXPECTED;
                }
            } else if (home_state == UNEXPECTED) {
                if (found_home) {
                  count_unexpected_home++;
#if VERBOSE_LOGGING
                    Serial.print("VERBOSE: Unexpected home! At ");
                    Serial.print(current_step);
                    Serial.print(". Unexpected range ");
                    Serial.print(unexpected_home_start_step);
                    Serial.print('-');
                    Serial.print(unexpected_home_end_step);
                    Serial.print("; missed at ");
                    Serial.print(missed_home_step);
                    Serial.print(".\n");
#endif
                    reset_to_home = true;
                } else if (current_step == unexpected_home_end_step) {
                    home_state = EXPECTED;
                }
            } else if (home_state == EXPECTED) {
                if (found_home) {
#if VERBOSE_LOGGING
                    Serial.print("VERBOSE: Found expected home.");
#endif
                    UpdateExpectedHome();
                } else if (current_step == missed_home_step) {
                  count_missed_home++;
#if VERBOSE_LOGGING
                    Serial.print("VERBOSE: Missed expected home! At ");
                    Serial.print(current_step);
                    Serial.print(". Expected between ");
                    Serial.print(unexpected_home_end_step);
                    Serial.print(" and ");
                    Serial.print(missed_home_step);
                    Serial.print(".\n");
#endif
                    reset_to_home = true;
                }
            }
#endif

            if (reset_to_home) {
                GoHome();
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
            if (found_home) {
#if VERBOSE_LOGGING
                Serial.print("VERBOSE: Found home!\n");
#endif
                state = NORMAL;
                target_accel_step = 0;

                // Reset frame of reference
                current_step = 0;
                unexpected_home_start_step = 0;
                unexpected_home_end_step = 0;
                missed_home_step = 0;
                UpdateExpectedHome();

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
            if (current_step == GEAR_RATIO_INPUT_STEPS) {
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
        if (current_step >= GEAR_RATIO_INPUT_STEPS) {
            Panic("current_step >= GEAR_RATIO_INPUT_STEPS");
        }
#endif
    }
    return false;
}

void SplitflapModule::ResetErrorCounters() {
  count_unexpected_home = 0;
  count_missed_home = 0;
}

void SplitflapModule::Init() {
    CheckSensor();
}

bool SplitflapModule::GetHomeState() {
  return (sensor_in & sensor_bitmask) != 0;
}

#endif
