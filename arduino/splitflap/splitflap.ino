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

#define DEBUG_LED_0_PIN 13
#define DEBUG_LED_1_PIN 5

#define MOT_C_PHASE_A B10000000
#define MOT_C_PHASE_B B01000000
#define MOT_C_PHASE_C B00010000
#define MOT_C_PHASE_D B00100000

#define MOT_D_PHASE_A B01000000
#define MOT_D_PHASE_B B00100000
#define MOT_D_PHASE_C B00010000
#define MOT_D_PHASE_D B10000000

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

SplitflapModule moduleC(flaps, step_pattern_C, DDRD, PORTD, 0xF0, DDRF, PORTF, PINF, B00100000);
SplitflapModule moduleD(flaps, step_pattern_D, DDRB, PORTB, 0xF0, DDRF, PORTF, PINF, B00010000);


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
  moduleC.init();
  moduleD.init();
  moduleC.goHome();
  moduleD.goHome();
  digitalWrite(DEBUG_LED_0_PIN, LOW);
}

void loop() {
  if (Serial.available() > 0) {
    int b = Serial.read();
    switch (b) {
      case '@':
        moduleC.goHome();
        moduleD.goHome();
        break;
      default:
        moduleC.goToFlap(b);
        moduleD.goToFlap(b);
        break;
    }
  }
  moduleC.update();
  moduleD.update();
}
