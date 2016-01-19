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

#ifndef SPLITFLAP_MODULE_H
#define SPLITFLAP_MODULE_H

#include <Arduino.h>

#define NUM_FLAPS (40)
#define STEPS_PER_REVOLUTION (64.0 * 32)
//(63.68395 * 64)
#define STEPS_PER_FLAP (STEPS_PER_REVOLUTION / NUM_FLAPS)

#define MAX_PERIOD_MICROS (20000)
#define MIN_PERIOD_MICROS (1200)

#define ACCEL_TIME_MICROS (200000)
#define MAX_RAMP_LEVELS (ACCEL_TIME_MICROS/MIN_PERIOD_MICROS)

class SplitflapModule {

private:
  const int (&flaps)[NUM_FLAPS];
  const uint8_t (&stepPattern)[4];
  volatile uint8_t &ddr;
  volatile uint8_t &port;
  const uint8_t mask;
  
  int RAMP_PERIODS[MAX_RAMP_LEVELS+2];
  int lastHome;
  long current = 0;
  float desired = 0;
  int desiredFlapIndex = 0;
  
  unsigned long lastUpdateMicros = 0;
  int computedMaxRampLevel;
  int curRampLevel = 0;
  long stepPeriod = 0;

  int findFlapIndex(int character);
  void panic();
  void computeAccelerationRamp();
  
public:
  SplitflapModule(
    const int (&flaps)[NUM_FLAPS],
    const uint8_t (&stepPattern)[4],
    volatile uint8_t &ddr,
    volatile uint8_t &port,
    const uint8_t mask
  );
  void update();
  void goHome();
  bool goToFlap(int character);
  void init();
  void testing();
};

#endif
