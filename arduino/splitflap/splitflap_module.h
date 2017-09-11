/*
   Copyright 2015-2016 Scott Bezek and the splitflap contributors

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

#define VERBOSE_LOGGING false

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

// Enable for auto-calibration via home sensor feedback. Disable for basic open-loop control.
#define HOME_CALIBRATION_ENABLED true
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
  void GoHome();
  bool Update();
  void Init();
  
  uint8_t count_unexpected_home = 0;
  uint8_t count_missed_home = 0;
};

#endif
