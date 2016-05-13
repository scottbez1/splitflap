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

#include <avr/power.h>
#include <math.h>
#include "splitflap_module.h"

// Prevent serial comms from attempting to toggle rx/tx led pins
#define TX_RX_LED_INIT
#define TXLED1
#define TXLED0
#define RXLED1
#define RXLED0

const int flaps[] = {
  ' ',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  '.',
  ',',
  '\'',
};

// TODO: precompute all of this
#define MAX_PERIOD_MICROS (20000)
#define MIN_PERIOD_MICROS (1200)
#define ACCEL_TIME_MICROS (200000)
#define MAX_RAMP_LEVELS (ACCEL_TIME_MICROS/MIN_PERIOD_MICROS)
int RAMP_PERIODS[MAX_RAMP_LEVELS+2];

#define DEBUG_LED_0_PIN 13
#define DEBUG_LED_1_PIN 5


#define MOT_A_PHASE_A B00001000
#define MOT_A_PHASE_B B00000100
#define MOT_A_PHASE_C B00000010
#define MOT_A_PHASE_D B00000001

#define MOT_B_PHASE_A B00001000
#define MOT_B_PHASE_B B00000100
#define MOT_B_PHASE_C B00000010
#define MOT_B_PHASE_D B00000001

#define MOT_C_PHASE_A B10000000
#define MOT_C_PHASE_B B01000000
#define MOT_C_PHASE_C B00010000
#define MOT_C_PHASE_D B00100000

#define MOT_D_PHASE_A B01000000
#define MOT_D_PHASE_B B00100000
#define MOT_D_PHASE_C B00010000
#define MOT_D_PHASE_D B10000000

const uint8_t step_pattern_A[] = {
  MOT_A_PHASE_D | MOT_A_PHASE_A,
  MOT_A_PHASE_C | MOT_A_PHASE_D,
  MOT_A_PHASE_B | MOT_A_PHASE_C,
  MOT_A_PHASE_A | MOT_A_PHASE_B,
};

const uint8_t step_pattern_B[] = {
  MOT_B_PHASE_D | MOT_B_PHASE_A,
  MOT_B_PHASE_C | MOT_B_PHASE_D,
  MOT_B_PHASE_B | MOT_B_PHASE_C,
  MOT_B_PHASE_A | MOT_B_PHASE_B,
};

const uint8_t step_pattern_C[] = {
  MOT_C_PHASE_D | MOT_C_PHASE_A,
  MOT_C_PHASE_C | MOT_C_PHASE_D,
  MOT_C_PHASE_B | MOT_C_PHASE_C,
  MOT_C_PHASE_A | MOT_C_PHASE_B,
};

const uint8_t step_pattern_D[] = {
  MOT_D_PHASE_D | MOT_D_PHASE_A,
  MOT_D_PHASE_C | MOT_D_PHASE_D,
  MOT_D_PHASE_B | MOT_D_PHASE_C,
  MOT_D_PHASE_A | MOT_D_PHASE_B,
};

SplitflapModule moduleA(flaps, step_pattern_A, DDRB, PORTB, 0x0F, DDRF, PORTF, PINF, B10000000, RAMP_PERIODS);
SplitflapModule moduleB(flaps, step_pattern_B, DDRD, PORTD, 0x0F, DDRF, PORTF, PINF, B01000000, RAMP_PERIODS);
SplitflapModule moduleC(flaps, step_pattern_C, DDRD, PORTD, 0xF0, DDRF, PORTF, PINF, B00100000, RAMP_PERIODS);
SplitflapModule moduleD(flaps, step_pattern_D, DDRB, PORTB, 0xF0, DDRF, PORTF, PINF, B00010000, RAMP_PERIODS);


void setup() {
  clock_prescale_set(clock_div_1);

  // Disable JTAG (shares port F). Have to write JTD bit twice in four cycles.
  MCUCR |= (1<<JTD);
  MCUCR |= (1<<JTD);

  Serial.begin(115200);
  
  pinMode(DEBUG_LED_0_PIN, OUTPUT);
  pinMode(DEBUG_LED_1_PIN, OUTPUT);

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
  int max_ramp_level = computeAccelerationRamp();
  moduleA.init(max_ramp_level);
  moduleB.init(max_ramp_level);
  moduleC.init(max_ramp_level);
  moduleD.init(max_ramp_level);
  moduleA.goHome();
  moduleB.goHome();
  moduleC.goHome();
  moduleD.goHome();
  digitalWrite(DEBUG_LED_0_PIN, LOW);
}

#define NUM_MODULES 4
int recv_buffer[] = {0, 0, 0, 0};
int recv_count = 0;
void loop() {
  if (Serial.available() > 0) {
    int b = Serial.read();
    switch (b) {
      case '@':
        moduleA.goHome();
        moduleB.goHome();
        moduleC.goHome();
        moduleD.goHome();
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
          moduleB.goToFlap(recv_buffer[1]);
          moduleC.goToFlap(recv_buffer[2]);
          moduleD.goToFlap(recv_buffer[3]);
        }
        break;
    }
  }
  moduleA.update();
  moduleB.update();
  moduleC.update();
  moduleD.update();
}

int computeAccelerationRamp() {
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
      Serial.print("failed to compute acceleration ramp");
      while (true) {
        digitalWrite(DEBUG_LED_0_PIN, HIGH);
        delay(100);
        digitalWrite(DEBUG_LED_0_PIN, LOW);
        delay(100);
      }
    }
  }
  int computedMaxRampLevel = i - 1;
  Serial.print("]\n\nVelocity steps: ");
  Serial.print(computedMaxRampLevel);
  Serial.print("\n\n");
  return computedMaxRampLevel;
}
