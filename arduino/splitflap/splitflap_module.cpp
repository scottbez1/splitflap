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
      panic();
    }
  }
  computedMaxRampLevel = i - 1;
  Serial.print("]\n\nVelocity steps: ");
  Serial.print(computedMaxRampLevel);
  Serial.print("\n\n");
}

void SplitflapModule::panic() {
  motor_port &= ~(motor_mask);
  pinMode(13, OUTPUT);
  while (1) {
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(100);
  }
}

inline bool SplitflapModule::sensorTriggered() {
    int curHome = readSensor();
    bool shift = curHome == HIGH && lastHome == LOW;
    lastHome = curHome;

    return shift;
}

void SplitflapModule::goHome() {
  boolean foundHome = false;

  for (int i = 0; i < STEPS_PER_REVOLUTION + STEPS_PER_FLAP * 5; i++) {

    if (sensorTriggered()) {
      foundHome = true;
      break;
    }

    motor_port = stepPattern[i & B11];
    delayMicroseconds(RAMP_PERIODS[computedMaxRampLevel/6]);
  }
  motor_port = 0;

  if (!foundHome) {
    panic();
  }
  
  currentFlapIndex = 0;
  desired = current;
  curRampLevel = 0;
  lookForHome = false;
}

void SplitflapModule::testing() {
  desired += STEPS_PER_REVOLUTION;
}

bool SplitflapModule::goToFlap(int character) {
  int flapIndex = findFlapIndex(character);
  if (flapIndex != -1) {
    goToFlapIndex(flapIndex);
    return true;
  } else {
    return false;
  }
}

void SplitflapModule::goToFlapIndex(int flapIndex) {
    int deltaFlaps = flapIndex - desiredFlapIndex;

    // Always only travel in the forward direction
    if (deltaFlaps <= 0) {
      deltaFlaps += NUM_FLAPS;
    }
    desiredFlapIndex = (desiredFlapIndex + deltaFlaps) % NUM_FLAPS;
    desired += STEPS_PER_FLAP * deltaFlaps;
}

void SplitflapModule::update() {
  if (sensorTriggered() && lookForHome) {
    lookForHome = false;

    Serial.print("Hit home. current flap=");
    Serial.print(currentFlapIndex);
    if (currentFlapIndex > 0.5 && currentFlapIndex < NUM_FLAPS - 0.5) {
      Serial.write("    *** CORRECTING ***");
      // TODO: adjust `desired` accordingly to rotate the frame of reference, rather than giving up and going home...
      goHome();
      int goTo = desiredFlapIndex;
      desiredFlapIndex = 0;
      goToFlapIndex(goTo);
    }
    Serial.write('\n');
  }

  if (currentFlapIndex > 1.5 && currentFlapIndex < (NUM_FLAPS / 4) && lookForHome) {
    Serial.print("Missed expected home *** CORRECTING ***\n");
    goHome();
    int goTo = desiredFlapIndex;
    desiredFlapIndex = 0;
    goToFlapIndex(goTo);
  }
  
  unsigned long now = micros();
  if (now - lastUpdateMicros >= stepPeriod) {
    if (curRampLevel > 0) {
      current++;
      currentFlapIndex += FLAPS_PER_STEP;
    } else if (curRampLevel < 0) {
      current--;
      currentFlapIndex -= FLAPS_PER_STEP;
    }
    lastUpdateMicros = now;
    
    while (currentFlapIndex > NUM_FLAPS - 0.001) {
      currentFlapIndex -= NUM_FLAPS;
    }
    while (currentFlapIndex < -NUM_FLAPS + 0.001) {
      currentFlapIndex += NUM_FLAPS;
    }

    if (currentFlapIndex > NUM_FLAPS / 2 && currentFlapIndex < NUM_FLAPS * 3 / 4) {
      lookForHome = true;
    }
    
    float delta = desired - current;
    if (delta > -1 && delta < 1) {
      curRampLevel = 0;
      stepPeriod = 0;
      motor_port &= ~(motor_mask);

      while (current > 64) {
        current -= 64;
        desired -= 64;
      }
      while (current < -64) {
        current += 64;
        desired += 64;
      }
      while (currentFlapIndex > NUM_FLAPS - 1.001) {
        currentFlapIndex -= NUM_FLAPS;
      }
      while (currentFlapIndex < -NUM_FLAPS + 1.001) {
        currentFlapIndex += NUM_FLAPS;
      }
      return;
    }

//    if (port == 0) {
//      port = stepPattern[current & B11];
//      delay(10);
//      port = stepPattern[(current-1) & B11];
//      delay(10);
//      port = stepPattern[(current-2) & B11];
//      delay(10);
//      port = stepPattern[(current-3) & B11];
//      delay(10);
//      port = stepPattern[(current-2) & B11];
//      delay(10);
//      port = stepPattern[(current-1) & B11];
//      delay(10);
//      port = stepPattern[current & B11];
//      delay(10);
//    }

    motor_port = (motor_port & ~(motor_mask)) | stepPattern[current & B11];

    int desiredRampLevel = 0;
    if (delta > computedMaxRampLevel) {
      desiredRampLevel = computedMaxRampLevel;
    } else if (delta < -computedMaxRampLevel) {
      desiredRampLevel = -computedMaxRampLevel;
    } else {
      desiredRampLevel = (int)delta;
    }
    if (curRampLevel > desiredRampLevel) {
      curRampLevel--;
    } else if (curRampLevel < desiredRampLevel) {
      curRampLevel++;
    }

    stepPeriod = curRampLevel > 0 ? RAMP_PERIODS[curRampLevel] : RAMP_PERIODS[-curRampLevel];
  }
}


