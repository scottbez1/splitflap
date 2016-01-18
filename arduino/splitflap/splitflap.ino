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

#include <math.h>
#include "splitflap_module.h"

bool initializing = false;
long initStartMicros;


const int flaps[] = {
  ' ',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  '.',
  ',',
  '\'',
};

const uint8_t step_pattern[] = {
  B00001001,
  B00001100,
  B00000110,
  B00000011,
};

SplitflapModule moduleA(flaps, step_pattern, DDRD, PORTD);

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  Serial.begin(115200);

//  while(!Serial) {}
  
  digitalWrite(13, HIGH);
  moduleA.init();
  moduleA.testing();
  digitalWrite(13, LOW);

  initializing = true;
  initStartMicros = micros();
}


void loop() {
  if (Serial.available() > 0) {
    int b = Serial.read();
    switch (b) {
//      case '{':
//        desired += STEPS_PER_FLAP / 4;
//        break;
//      case '[':
//        desired += STEPS_PER_FLAP;
//        break;
//      case ']':
//        desired -= STEPS_PER_FLAP;
//        break;
//      case '}':
//        desired -= STEPS_PER_FLAP / 4;
//        break;
//      case '!':
//        if (port == 0) {
//          desiredFlapIndex = 0;
//        }
//        break;
      case '@':
        moduleA.goHome();
        break;
      default:
        moduleA.goToFlap(b);
        break;
    }
  }
  moduleA.update();
}
