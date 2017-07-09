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

#include <avr/power.h>
#include <math.h>
#include <SPI.h>
#include "acceleration.h"
#include "splitflap_module.h"

const int flaps[] = {
  ' ',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  '.',
  ',',
  '\'',
};

#define DEBUG_LED_0_PIN (9)
#define DEBUG_LED_1_PIN (10)
#define OUT_LATCH_PIN (7)
#define IN_LATCH_PIN (8)


#define MOT_A_PHASE_A B00001000
#define MOT_A_PHASE_B B00000100
#define MOT_A_PHASE_C B00000010
#define MOT_A_PHASE_D B00000001

const uint8_t step_pattern_A[] = {
  MOT_A_PHASE_D | MOT_A_PHASE_A,
  MOT_A_PHASE_C | MOT_A_PHASE_D,
  MOT_A_PHASE_B | MOT_A_PHASE_C,
  MOT_A_PHASE_A | MOT_A_PHASE_B,
};

#define MOTOR_BUFFER_LENGTH (1)
uint8_t motor_buffer[1];

#define SENSOR_BUFFER_LENGTH (1)
uint8_t sensor_buffer[1];

SplitflapModule moduleA(flaps, step_pattern_A, motor_buffer[0], 0, sensor_buffer[0], B00000001, Acceleration::RAMP_PERIODS, Acceleration::NUM_RAMP_LEVELS);

inline void spi_transfer() {
  digitalWrite(IN_LATCH_PIN, LOW);
  digitalWrite(IN_LATCH_PIN, HIGH);

  for (int i = 0; i < MOTOR_BUFFER_LENGTH; i++) {
    int val = SPI.transfer(motor_buffer[0]);
    if (i < SENSOR_BUFFER_LENGTH) {
      sensor_buffer[i] = val;
    }
  }

  digitalWrite(OUT_LATCH_PIN, HIGH);
  digitalWrite(OUT_LATCH_PIN, LOW);
}

void setup() {
  Serial.begin(115200);
  
  pinMode(DEBUG_LED_0_PIN, OUTPUT);
  pinMode(DEBUG_LED_1_PIN, OUTPUT);

  for (int i = 0; i < MOTOR_BUFFER_LENGTH; i++) {
    motor_buffer[i] = 0;
  }
  for (int i = 0; i < SENSOR_BUFFER_LENGTH; i++) {
    sensor_buffer[i] = 0;
  }

  // Initialize SPI
  pinMode(IN_LATCH_PIN, OUTPUT);
  pinMode(OUT_LATCH_PIN, OUTPUT);
  digitalWrite(IN_LATCH_PIN, HIGH);
  SPI.begin();
  SPI.beginTransaction(SPISettings(3000000, MSBFIRST, SPI_MODE0));
  spi_transfer();

  Serial.print("Starting.\n");

  // Pulse DEBUG_LED_1_PIN for fun
  for (int i = 0; i < 255; i++) {
    analogWrite(DEBUG_LED_1_PIN, i);
    delay(3);
  }
  for (int i = 255; i >= 0; i--) {
    analogWrite(DEBUG_LED_1_PIN, i);
    delay(3);
  }

  digitalWrite(DEBUG_LED_0_PIN, HIGH);
  moduleA.init();
  moduleA.goHome();
  digitalWrite(DEBUG_LED_0_PIN, LOW);
}

#define NUM_MODULES 1
int recv_buffer[] = {0, 0, 0, 0};
int recv_count = 0;
void loop() {
  if (Serial.available() > 0) {
    int b = Serial.read();
    switch (b) {
      case '@':
        moduleA.goHome();
        break;
      case '=':
        recv_count = 0;
        break;
      default:
        if (recv_count > NUM_MODULES - 1) {
          break;
        }
        recv_buffer[recv_count] = b;
        recv_count++;
        if (recv_count == NUM_MODULES) {
          moduleA.goToFlap(recv_buffer[0]);
        }
        break;
    }
  }
  moduleA.update();
  spi_transfer();
}

