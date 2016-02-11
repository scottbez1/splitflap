/*
   Copyright 2015 Scott Bezek

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
  volatile uint8_t &motor_ddr,
  volatile uint8_t &motor_port,
  const uint8_t motor_mask,
  volatile uint8_t &sensor_ddr,
  volatile uint8_t &sensor_port,
  volatile uint8_t &sensor_pin,
  const uint8_t sensor_mask) : 
    flaps(flaps),
    stepPattern(stepPattern),
    motor_ddr(motor_ddr),
    motor_port(motor_port),
    motor_mask(motor_mask),
    sensor_ddr(sensor_ddr),
    sensor_port(sensor_port),
    sensor_pin(sensor_pin),
    sensor_mask(sensor_mask)
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

bool SplitflapModule::readSensor() {
  return (sensor_pin & sensor_mask) != 0;
}

void SplitflapModule::init() {
  motor_ddr |= motor_mask;

  // Set up sensor as input and enable internal pull-up
  sensor_ddr &= ~sensor_mask;
  sensor_port |= sensor_mask;
  delay(5);
  lastHome = readSensor();
  
  computeAccelerationRamp();
}

void SplitflapModule::computeAccelerationRamp() {
  float minVel = 1000000. / MAX_PERIOD_MICROS;
  float maxVel = 1000000. / MIN_PERIOD_MICROS;
  long t = 0;
  int i = 1;
  RAMP_PERIODS[0] = 0;
  Serial.print("Computing acceleration table: \n[\n");
  while (t < ACCEL_TIME_MICROS) {
    float vel = minVel + (maxVel - minVel) * ((float)t/ACCEL_TIME_MICROS);
    if (vel > maxVel) {
      vel = maxVel;
    }
    int period = (int)(1000000. / vel);

    Serial.print(period);
    Serial.print(",\n");

    RAMP_PERIODS[i] = period;
    t += period;
    i++;

    if (i >= MAX_RAMP_LEVELS + 1) {
      panic("failed to compute acceleration ramp");
      break;
    }
  }
  computedMaxRampLevel = i - 1;
  Serial.print("]\n\nVelocity steps: ");
  Serial.print(computedMaxRampLevel);
  Serial.print("\n\n");
}

void SplitflapModule::panic(char* message) {
  Serial.print("#### PANIC! ####\n");
  Serial.print(message);
  motor_port &= ~(motor_mask);
  state = PANIC;
}

inline bool SplitflapModule::sensorTriggered() {
    int curHome = readSensor();
    bool shift = curHome == HIGH && lastHome == LOW;
    lastHome = curHome;

    return shift;
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

void SplitflapModule::update() {
  if (state == PANIC) {
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

    bool foundHome = sensorTriggered();
    int desiredRampLevel = 0;
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
          }
        } else if (currentFlapIndex > 0.5 && currentFlapIndex < 5) {
          Serial.print("Missed expected home *** CORRECTING ***\n");
          goHome();
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
        return;
      } else if (delta < 1) {
        desiredRampLevel = 0;
  
        while (current > 64) {
          current -= 64;
        }
        while (currentFlapIndex > NUM_FLAPS - 1.001) {
          currentFlapIndex -= NUM_FLAPS;
        }
      } else if (delta > computedMaxRampLevel) {
        desiredRampLevel = computedMaxRampLevel;
      } else {
        desiredRampLevel = (int)delta;
      }
    } else if (state == RESET_TO_HOME) {
      if (foundHome) {
        state = NORMAL;
        currentFlapIndex = 0;
        lookForHome = false;
        
        desiredRampLevel = 0;
      } else {
        if (++stepsLookingForHome > (NUM_FLAPS + 5) * STEPS_PER_FLAP) {
          Serial.print("Never found home position. Entering disabled state.\n");
          state = SENSOR_ERROR;
        } else  {
          desiredRampLevel = computedMaxRampLevel / 6;
        }
      }
    }

    // Update current velocity based on desired velocity
    if (curRampLevel > desiredRampLevel) {
      curRampLevel--;
    } else if (curRampLevel < desiredRampLevel) {
      curRampLevel++;
    }

    // Set motor step outputs based on current step
    if (curRampLevel == 0) {
      stepPeriod = 0;
      motor_port &= ~(motor_mask);
    } else {
      stepPeriod = curRampLevel > 0 ? RAMP_PERIODS[curRampLevel] : RAMP_PERIODS[-curRampLevel];
      motor_port = (motor_port & ~(motor_mask)) | stepPattern[current & B11];
    }

    lastUpdateMicros = now;
  }
}


