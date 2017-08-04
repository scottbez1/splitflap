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
#include "splitflap_module.h"

SplitflapModule::SplitflapModule(
  const uint8_t (&flaps)[NUM_FLAPS],
  const uint8_t (&step_pattern)[4],
  uint8_t &motor_out,
  const uint8_t motor_bitshift,
  uint8_t &sensor_in,
  const uint8_t sensor_bitmask) :
    flaps(flaps),
    step_pattern(step_pattern),
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

bool SplitflapModule::CheckSensor() {
    bool cur_home = (sensor_in & sensor_bitmask) != 0;
    bool shift = cur_home == true && last_home == false;
    last_home = cur_home;

    return shift;
}

inline void SplitflapModule::SetMotor(uint8_t out) {
  motor_out = (motor_out & ~(0x0F << motor_bitshift)) | ((out & 0x0F) << motor_bitshift);
}

inline uint8_t SplitflapModule::GetFlapFloor(uint32_t step) {
    return step * GEAR_RATIO_OUTPUT_FLAPS / GEAR_RATIO_INPUT_STEPS;
}

uint32_t SplitflapModule::GetTargetStepForFlapIndex(uint32_t from_step, uint8_t target_flap_index) {
    //assert 0 <= from_step < 2*GEAR_RATIO_INPUT_STEPS
    if (from_step < 0 || from_step >= 2 * GEAR_RATIO_INPUT_STEPS) {
        Panic("from_step < 0 || from_step >= 2 * GEAR_RATIO_INPUT_STEPS");
    }

    uint8_t from_flap = GetFlapFloor(from_step);
    //assert 0 <= from_flap < 2*NUM_FLAPS
    if (from_flap < 0 || from_flap >= 2 * NUM_FLAPS) {
        Panic("from_flap < 0 || from_flap >= 2 * NUM_FLAPS");
    }

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
    Serial.print("VERBOSE: DELTA FLAPS=");
    Serial.print(delta_flaps);
    Serial.print('\n');

    //assert 0 < delta_flaps <= 40
    if (delta_flaps <= 0 || delta_flaps > NUM_FLAPS) {
        Panic("delta_flaps <= 0 || delta_flaps > NUM_FLAPS");
    }

    uint32_t gear_ratio_input_steps_flaps_destination = ((uint32_t)from_flap + (uint32_t)delta_flaps) * GEAR_RATIO_INPUT_STEPS;

    // Round UP when dividing so that the inverse calculation on the result (_get_flap_floor) returns the expected
    // result.
    uint32_t result = gear_ratio_input_steps_flaps_destination / GEAR_RATIO_OUTPUT_FLAPS;
    if (gear_ratio_input_steps_flaps_destination % GEAR_RATIO_OUTPUT_FLAPS != 0) {
        result++;
    }
    return result;
}

void SplitflapModule::GoToTargetFlapIndex() {
    delta_steps = GetTargetStepForFlapIndex(current_step, target_flap_index) - current_step;


    Serial.print("Going to flap index ");
    Serial.print(target_flap_index);
    Serial.print(". Current step is ");
    Serial.print(current_step);
    Serial.print(". Delta is ");
    Serial.print(delta_steps);
    Serial.print('\n');
//    logger.debug('Going to flap index {}. Current step is {}. Delta is {}.'.format(
//        self.target_flap_index,
//        self.current_step,
//        self.delta_steps,
//    ))
//
//    assert self.delta_steps >= 0
//    assert self.delta_steps <= GEAR_RATIO_INPUT_STEPS
    if (delta_steps > GEAR_RATIO_INPUT_STEPS) {
        Panic("delta_steps > GEAR_RATIO_INPUT_STEPS");
    }
}

void SplitflapModule::UpdateExpectedHome() {
#if HOME_CALIBRATION_ENABLED
    #error "TODO"
#endif
}

void SplitflapModule::GoToFlap(uint8_t character) {
    if (state != NORMAL || current_accel_step != 0) {
        return;
    }

    for (uint8_t i = 0; i < NUM_FLAPS; i++) {
        if (character == flaps[i]) {
            target_flap_index = i;
            GoToTargetFlapIndex();
            return;
        }
    }
}

void SplitflapModule::GoHome() {
#if HOME_CALIBRATION_ENABLED
    #error "TODO"
#endif
}

bool SplitflapModule::Update() {
    unsigned long now = micros();
    unsigned long delta_time = now - last_update_micros;
    if (delta_time >= current_period) {
        last_update_micros = now;

        uint8_t target_accel_step;

        if (state == NORMAL) {
            bool reset_to_home = false;
#if HOME_CALIBRATION_ENABLED
            #error "TODO"
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
        } else if (state == LOOK_FOR_HOME) {
#if HOME_CALIBRATION_ENABLED
            #error "TODO"
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

        current_period = Acceleration::ACCEL_STEP_PERIODS[current_accel_step];

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


//        logger.info('update: cs={}, delta={}, cp={}, us={}, ue={}, mh={}, speed={}, period={}, [flap {!r}]'.format(
//            self.current_step,
//            self.delta_steps,
//            self.current_phase,
//            self.unexpected_home_start_step,
//            self.unexpected_home_end_step,
//            self.missed_home_step,
//            self.current_speed,
//            self.current_period,
//            FLAPS[self._get_flap_floor(self.current_step) % NUM_FLAPS],
//        ))

        // Check modular arithmetic invariant
//        assert self.current_step < GEAR_RATIO_INPUT_STEPS
        if (current_step >= GEAR_RATIO_INPUT_STEPS) {
            Panic("current_step >= GEAR_RATIO_INPUT_STEPS");
        }
//        assert self.current_step >= 0
    }
    return false;
}

void SplitflapModule::Init() {
    CheckSensor();
}
