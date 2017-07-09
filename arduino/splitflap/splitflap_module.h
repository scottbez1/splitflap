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

#define NUM_FLAPS (40)
#define STEPS_PER_REVOLUTION (64.0 * 32)
//(63.68395 * 64)
#define STEPS_PER_FLAP (STEPS_PER_REVOLUTION / NUM_FLAPS)
#define FLAPS_PER_STEP ((float)NUM_FLAPS / STEPS_PER_REVOLUTION)


class SplitflapModule {

private:

  enum State {
    NORMAL,
    RESET_TO_HOME,
    SENSOR_ERROR,
    PANIC,
  };

  // Configuration:
  const int (&flaps)[NUM_FLAPS];
  const uint8_t (&stepPattern)[4];
  
  uint8_t &motor_out;
  const uint8_t motor_bitshift;

  uint8_t &sensor_in;
  const uint8_t sensor_bitmask;
  
  const int* const ramp_periods;
  const int num_ramp_levels;

  // State:
  long current = 0;
  
  int desiredFlapIndex = 0;
  float currentFlapIndex = 0;
  
  int lastHome;
  bool lookForHome = false;

  State state = NORMAL;
  long stepsLookingForHome = 0;
  
  unsigned long lastUpdateMicros = 0;

  int curRampLevel = 0;
  long stepPeriod = 0;

  int findFlapIndex(int character);
  void panic(String message);
  bool checkSensor();
  int determineDesiredRampLevel();
  void setMotor(uint8_t out);
  void goToFlapIndex(int flapIndex);
  
public:
  SplitflapModule(
    const int (&flaps)[NUM_FLAPS],
    const uint8_t (&stepPattern)[4],
    uint8_t &motor_out,
    const uint8_t motor_bitshift,
    uint8_t &sensor_in,
    const uint8_t sensor_bitmask,
    const int* const ramp_periods,
    const int num_ramp_levels
  );
  void update();
  void goHome();
  bool goToFlap(int character);
  void init();
};

#endif
