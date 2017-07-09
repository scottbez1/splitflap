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
  const int (&flaps)[NUM_FLAPS],
  const uint8_t (&stepPattern)[4],
  uint8_t &motor_out,
  const uint8_t motor_bitshift,
  uint8_t &sensor_in,
  const uint8_t sensor_bitmask,
  const int* const ramp_periods,
  const int num_ramp_levels) :
    flaps(flaps),
    stepPattern(stepPattern),
    motor_out(motor_out),
    motor_bitshift(motor_bitshift),
    sensor_in(sensor_in),
    sensor_bitmask(sensor_bitmask),
    ramp_periods(ramp_periods),
    num_ramp_levels(num_ramp_levels)
{
}

int SplitflapModule::findFlapIndex(int character) {
  for (int i = 0; i < NUM_FLAPS; i++) {
    if (character == flaps[i]) {
      return i;
    }
  }
  return -1;
}

void SplitflapModule::init() {
  // Consume the intial sensor state. If we we happen to already be "home" when
  // we start, we wouldn't want to treat the first sensor read as a rising edge
  // (since we might be halfway inside the home region rather than at the
  // beginning of it), so we read the sensor and throw away the reading.
  checkSensor();
}

void SplitflapModule::panic(String message) {
  Serial.print("#### PANIC! ####\n");
  Serial.print(message);
  setMotor(0);
  state = PANIC;
}

inline bool SplitflapModule::checkSensor() {
    int curHome = (sensor_in & sensor_bitmask) != 0;
    bool shift = curHome == HIGH && lastHome == LOW;
    lastHome = curHome;

    return shift;
}

inline void SplitflapModule::setMotor(uint8_t out) {
  motor_out = (motor_out & ~(0x0F << motor_bitshift)) | ((out & 0x0F) << motor_bitshift);
}

void SplitflapModule::goHome() {
  if (state == PANIC) {
    return;
  }
  stepsLookingForHome = 0;
  state = RESET_TO_HOME;
}

bool SplitflapModule::goToFlap(int character) {
  if (state == PANIC || state == SENSOR_ERROR) {
    return false;
  }
  int flapIndex = findFlapIndex(character);
  if (flapIndex != -1) {
    goToFlapIndex(flapIndex);
    return true;
  } else {
    return false;
  }
}

void SplitflapModule::goToFlapIndex(int flapIndex) {
  desiredFlapIndex = flapIndex;
}

int SplitflapModule::determineDesiredRampLevel() {
  bool foundHome = checkSensor();
  if (state == NORMAL) {
    if (lookForHome) {
      if (foundHome) {
        lookForHome = false;

        Serial.print("Hit home. current flap=");
        Serial.print(currentFlapIndex);
        Serial.write('\n');
        if (currentFlapIndex > 0.5 && currentFlapIndex < NUM_FLAPS - 0.5) {
          Serial.write("    *** CORRECTING ***\n");
          // TODO: adjust `desired` accordingly to rotate the frame of reference, rather than giving up and going home...
          goHome();
          panic("FIXME");
          return 0;
        }
      } else if (currentFlapIndex > 0.5 && currentFlapIndex < 5) {
        Serial.print("Missed expected home *** CORRECTING ***\n");
        goHome();
        panic("FIXME");
        return 0;
      }
    }
    if (currentFlapIndex > 5 && currentFlapIndex < NUM_FLAPS - 5) {
      lookForHome = true;
    }

    float deltaFlaps = desiredFlapIndex - currentFlapIndex;
    if (deltaFlaps < -0.25) {
      deltaFlaps += NUM_FLAPS;
    } else if (deltaFlaps < 0) {
      // If we're less than a quarter-flap past where we want to be, don't bother
      // doing a full rotation; just stop here.
      deltaFlaps = 0;
    }

    float delta = deltaFlaps * STEPS_PER_FLAP;
    if (delta < 0) {
      panic("negative delta");
      return 0;
    } else if (delta < 1) {
      while (current > 64) {
        current -= 64;
      }
      while (currentFlapIndex > NUM_FLAPS - 1.001) {
        currentFlapIndex -= NUM_FLAPS;
      }

      return 0;
    } else if (delta >= num_ramp_levels) {
      return num_ramp_levels - 1;
    } else {
      return (int)delta;
    }
  } else if (state == RESET_TO_HOME) {
    if (foundHome) {
      state = NORMAL;
      currentFlapIndex = 0;
      lookForHome = false;

      return 0;
    } else {
      if (++stepsLookingForHome > (NUM_FLAPS + 5) * STEPS_PER_FLAP) {
        Serial.print("Never found home position. Entering disabled state.\n");
        state = SENSOR_ERROR;
        return 0;
      } else  {
        return num_ramp_levels / 6;
      }
    }
  }
}

void SplitflapModule::update() {
  if (state == PANIC) {
    setMotor(0);
    return;
  }
  unsigned long now = micros();
  if (now - lastUpdateMicros >= stepPeriod) {
    if (curRampLevel > 0) {
      current++;
      currentFlapIndex += FLAPS_PER_STEP;
    } else if (curRampLevel < 0) {
      panic("negative velocity");
      return;
    }
    
    while (currentFlapIndex > NUM_FLAPS - 0.001) {
      currentFlapIndex -= NUM_FLAPS;
    }

    int desiredRampLevel = determineDesiredRampLevel();

    // Update current velocity based on desired velocity
    if (curRampLevel > desiredRampLevel) {
      curRampLevel--;
    } else if (curRampLevel < desiredRampLevel) {
      curRampLevel++;
    }

    // Set motor step outputs based on current step
    if (curRampLevel == 0) {
      stepPeriod = 0;
      setMotor(0);
    } else {
      stepPeriod = curRampLevel > 0 ? ramp_periods[curRampLevel] : ramp_periods[-curRampLevel];
      setMotor(stepPattern[current & B11]);
    }

    lastUpdateMicros = now;
  }
}


