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

SplitflapModule::SplitflapModule(const int (&flaps)[NUM_FLAPS], const uint8_t (&stepPattern)[4], volatile uint8_t &ddr, volatile uint8_t &port) : 
    flaps(flaps),
    stepPattern(stepPattern),
    ddr(ddr),
    port(port)
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
  ddr = 0xF;
  
  pinMode(31, INPUT);
  digitalWrite(31, HIGH);
  delay(5);
  lastHome = digitalRead(31);
  
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
  port = 0;
  pinMode(13, OUTPUT);
  while (1) {
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(100);
  }
}

void SplitflapModule::goHome() {
  boolean foundHome = false;

  for (int i = 0; i < STEPS_PER_REVOLUTION + STEPS_PER_FLAP * 5; i++) {
    int curHome = digitalRead(31);
    bool shift = curHome == HIGH && lastHome == LOW;
    lastHome = curHome;

    if (shift) {
      foundHome = true;
      break;
    }

    port = stepPattern[i & B11];
    delayMicroseconds(RAMP_PERIODS[computedMaxRampLevel/6]);
  }
  port = 0;

  if (!foundHome) {
    panic();
  }
  
  desiredFlapIndex = 0;
  current = (int)desired;
}

void SplitflapModule::testing() {
  desired += STEPS_PER_REVOLUTION;
}

bool SplitflapModule::goToFlap(int character) {
  int flapIndex = findFlapIndex(character);
  if (flapIndex != -1) {
    int deltaFlaps = flapIndex - desiredFlapIndex;

    // Always only travel in the forward direction
    if (deltaFlaps <= 0) {
      deltaFlaps += NUM_FLAPS;
    }
    desiredFlapIndex = (desiredFlapIndex + deltaFlaps) % NUM_FLAPS;
    desired += STEPS_PER_FLAP * deltaFlaps;
    
    return true;
  } else {
    return false;
  }
}

void SplitflapModule::update() {
  unsigned long now = micros();
  if (now - lastUpdateMicros >= stepPeriod) {
    if (curRampLevel > 0) {
      current++;
    } else if (curRampLevel < 0) {
      current--;
    }
    lastUpdateMicros = now;
      
    float delta = desired - current;
    if (delta > -1 && delta < 1) {
      curRampLevel = 0;
      stepPeriod = 0;
      port = 0;

      while (current > 64) {
        current -= 64;
        desired -= 64;
      }
      while (current < -64) {
        current += 64;
        desired += 64;
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

    port = stepPattern[current & B11];

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


