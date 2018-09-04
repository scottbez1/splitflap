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

#ifndef IO_CONFIG_H
#define IO_CONFIG_H

#define OUT_LATCH_PIN (4)
#define OUT_LATCH_PORT PORTD
#define OUT_LATCH_BIT 4

#define IN_LATCH_PIN (5)
#define IN_LATCH_PORT PORTD
#define IN_LATCH_BIT 5

#define MOTOR_BUFFER_LENGTH (NUM_MODULES / 2 + (NUM_MODULES % 2 != 0))
uint8_t motor_buffer[MOTOR_BUFFER_LENGTH];

#define SENSOR_BUFFER_LENGTH (NUM_MODULES / 4 + (NUM_MODULES % 4 != 0))
uint8_t sensor_buffer[SENSOR_BUFFER_LENGTH];

SplitflapModule moduleA(motor_buffer[0], 0, sensor_buffer[0], B00000001);
SplitflapModule moduleB(motor_buffer[0], 4, sensor_buffer[0], B00000010);
SplitflapModule moduleC(motor_buffer[1], 0, sensor_buffer[0], B00000100);
SplitflapModule moduleD(motor_buffer[1], 4, sensor_buffer[0], B00001000);

#if NUM_MODULES > 4
SplitflapModule moduleE(motor_buffer[2], 0, sensor_buffer[1], B00000001);
SplitflapModule moduleF(motor_buffer[2], 4, sensor_buffer[1], B00000010);
SplitflapModule moduleG(motor_buffer[3], 0, sensor_buffer[1], B00000100);
SplitflapModule moduleH(motor_buffer[3], 4, sensor_buffer[1], B00001000);
#endif

#if NUM_MODULES > 8
SplitflapModule moduleI(motor_buffer[4], 0, sensor_buffer[2], B00000001);
SplitflapModule moduleJ(motor_buffer[4], 4, sensor_buffer[2], B00000010);
SplitflapModule moduleK(motor_buffer[5], 0, sensor_buffer[2], B00000100);
SplitflapModule moduleL(motor_buffer[5], 4, sensor_buffer[2], B00001000);
#endif

SplitflapModule modules[] = {
  moduleA,
  moduleB,
  moduleC,
  moduleD,

#if NUM_MODULES > 4
  moduleE,
  moduleF,
  moduleG,
  moduleH,
#endif

#if NUM_MODULES > 8
  moduleI,
  moduleJ,
  moduleK,
  moduleL,
#endif
};


inline void motor_sensor_setup() {
  // Initialize SPI
  pinMode(IN_LATCH_PIN, OUTPUT);
  pinMode(OUT_LATCH_PIN, OUTPUT);
  digitalWrite(IN_LATCH_PIN, HIGH);
  SPI.begin();
  SPI.beginTransaction(SPISettings(3000000, MSBFIRST, SPI_MODE0));
  
  for (uint8_t i = 0; i < MOTOR_BUFFER_LENGTH; i++) {
    motor_buffer[i] = 0;
  }
  for (uint8_t i = 0; i < SENSOR_BUFFER_LENGTH; i++) {
    sensor_buffer[i] = 0;
  }
}

inline void motor_sensor_io() {
  IN_LATCH_PORT &= ~(1 << IN_LATCH_BIT);
  IN_LATCH_PORT |= (1 << IN_LATCH_BIT);

  for (uint8_t i = 0; i < MOTOR_BUFFER_LENGTH; i++) {
    int val = SPI.transfer(motor_buffer[MOTOR_BUFFER_LENGTH - 1 - i]);
    if (i < SENSOR_BUFFER_LENGTH) {
      sensor_buffer[i] = val;
    }
  }

  OUT_LATCH_PORT |= (1 << OUT_LATCH_BIT);
  OUT_LATCH_PORT &= ~(1 << OUT_LATCH_BIT);
}

#endif
