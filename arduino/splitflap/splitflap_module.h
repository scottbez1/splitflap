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
#define FLAPS_PER_STEP ((float)NUM_FLAPS / STEPS_PER_REVOLUTION)

#define MAX_PERIOD_MICROS (20000)
#define MIN_PERIOD_MICROS (1200)

#define ACCEL_TIME_MICROS (200000)
#define MAX_RAMP_LEVELS (ACCEL_TIME_MICROS/MIN_PERIOD_MICROS)

class SplitflapModule {

private:

  enum State {
    NORMAL,
    RESET_TO_HOME,
    SENSOR_ERROR,
    PANIC,
  };
  
  const int (&flaps)[NUM_FLAPS];
  const uint8_t (&stepPattern)[4];
  
  volatile uint8_t &motor_ddr;
  volatile uint8_t &motor_port;
  const uint8_t motor_mask;
  
  volatile uint8_t &sensor_ddr;
  volatile uint8_t &sensor_port;
  volatile uint8_t &sensor_pin;
  const uint8_t sensor_mask;
  
  int RAMP_PERIODS[MAX_RAMP_LEVELS+2];
  long current = 0;
  
  int desiredFlapIndex = 0;
  float currentFlapIndex = 0;
  
  int lastHome;
  bool lookForHome = false;

  State state = NORMAL;
  long stepsLookingForHome = 0;
  
  unsigned long lastUpdateMicros = 0;
  int computedMaxRampLevel;
  int curRampLevel = 0;
  long stepPeriod = 0;

  int findFlapIndex(int character);
  void panic(char* message);
  void computeAccelerationRamp();
  bool readSensor();
  bool sensorTriggered();
  void goToFlapIndex(int flapIndex);
  
public:
  SplitflapModule(
    const int (&flaps)[NUM_FLAPS],
    const uint8_t (&stepPattern)[4],
    volatile uint8_t &motor_ddr,
    volatile uint8_t &motor_port,
    const uint8_t motor_mask,
    volatile uint8_t &sensor_ddr,
    volatile uint8_t &sensor_port,
    volatile uint8_t &sensor_pin,
    const uint8_t sensor_mask
  );
  void update();
  void goHome();
  bool goToFlap(int character);
  void init();
};

#endif
