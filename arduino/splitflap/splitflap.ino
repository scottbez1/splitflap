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
#include "splitflap_module.h"

#define NEOPIXEL_DEBUGGING_ENABLED true

#if NEOPIXEL_DEBUGGING_ENABLED
#include <Adafruit_NeoPixel.h>
#endif

const uint8_t flaps[] = {
  ' ',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  '.',
  ',',
  '\'',
};

#define DEBUG_LED_1_PIN (10)
#define OUT_LATCH_PIN (4)
#define OUT_LATCH_PORT PORTD
#define OUT_LATCH_BIT 4


#define IN_LATCH_PIN (5)
#define IN_LATCH_PORT PORTD
#define IN_LATCH_BIT 5


#define NUM_MODULES (12)
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
int recv_buffer[NUM_MODULES];

#if NEOPIXEL_DEBUGGING_ENABLED
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_MODULES, 6, NEO_GRB + NEO_KHZ800);
uint32_t color_green = strip.Color(0, 30, 0);
uint32_t color_red = strip.Color(100, 0, 0);
uint32_t color_purple = strip.Color(15, 0, 15);
uint32_t color_orange = strip.Color(30, 7, 0);
#endif

inline void spi_transfer() {
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

void setup() {
  Serial.begin(57600);

  pinMode(DEBUG_LED_0_PIN, OUTPUT);
  pinMode(DEBUG_LED_1_PIN, OUTPUT);

  for (uint8_t i = 0; i < MOTOR_BUFFER_LENGTH; i++) {
    motor_buffer[i] = 0;
  }
  for (uint8_t i = 0; i < SENSOR_BUFFER_LENGTH; i++) {
    sensor_buffer[i] = 0;
  }

  // Initialize SPI
  pinMode(IN_LATCH_PIN, OUTPUT);
  pinMode(OUT_LATCH_PIN, OUTPUT);
  digitalWrite(IN_LATCH_PIN, HIGH);
  SPI.begin();
  SPI.beginTransaction(SPISettings(3000000, MSBFIRST, SPI_MODE0));
  spi_transfer();

  Serial.print(F("Starting.\nNum modules: "));
  Serial.print(NUM_MODULES);
  Serial.print(F("\nMotor buffer length: "));
  Serial.print(MOTOR_BUFFER_LENGTH);
  Serial.print(F("\nSensor buffer length: "));
  Serial.print(SENSOR_BUFFER_LENGTH);
  Serial.print('\n');

#if NEOPIXEL_DEBUGGING_ENABLED
  strip.begin();
  strip.show();
#endif

  // Pulse DEBUG_LED_1_PIN for fun
  digitalWrite(DEBUG_LED_0_PIN, HIGH);
  uint8_t vals[] = {0, 1, 3, 10, 16, 20, 24, 28, 28, 24, 20, 16, 10, 3, 1, 0};
  for (int16_t i = 0; i < 400; i++) {
    analogWrite(DEBUG_LED_1_PIN, i);
#if NEOPIXEL_DEBUGGING_ENABLED
    for (int j = 0; j < NUM_MODULES; j++) {
      int8_t index = i / 8 - j;
      if (index < 0) {
        index = 0;
      } else if (index > 15) {
        index = index % 16;
      }
      strip.setPixelColor(j, vals[index] * 2, vals[index] * 2, 0);
    }
    strip.show();
#endif
    delay(3);
  }

  for (uint8_t i = 0; i < NUM_MODULES; i++) {
    recv_buffer[i] = 0;
    modules[i].Init();
    modules[i].GoHome();
  }
  digitalWrite(DEBUG_LED_0_PIN, LOW);
}


inline int8_t FindFlapIndex(uint8_t character) {
    for (int8_t i = 0; i < NUM_FLAPS; i++) {
        if (character == flaps[i]) {
          return i;
        }
    }
    return -1;
}


bool was_idle = false;
bool was_stopped = false;
uint8_t recv_count = 0;


void loop() {
  while (1) {
    boolean all_idle = true;
    boolean all_stopped = true;
    boolean any_bad_timing = false;
    for (uint8_t i = 0; i < NUM_MODULES; i++) {
    DEBUG_LED_0_PORT |= (1 << DEBUG_LED_0_BIT);
      any_bad_timing |= modules[i].Update();
    DEBUG_LED_0_PORT &= ~(1 << DEBUG_LED_0_BIT);
      bool is_idle = modules[i].state == PANIC
#if HOME_CALIBRATION_ENABLED
        || modules[i].state == LOOK_FOR_HOME
        || modules[i].state == SENSOR_ERROR
#endif
        || (modules[i].state == NORMAL && modules[i].current_accel_step == 0);
      all_idle &= is_idle;
      all_stopped &= modules[i].current_accel_step == 0;
      if (i & 0b11) spi_transfer();
    }
    spi_transfer();

    if (all_idle) {

#if NEOPIXEL_DEBUGGING_ENABLED
      for (int i = 0; i < NUM_MODULES; i++) {
        uint32_t color;
        switch (modules[i].state) {
          case NORMAL:
            color = color_green;
            break;
#if HOME_CALIBRATION_ENABLED
          case LOOK_FOR_HOME:
            color = color_purple;
            break;
          case SENSOR_ERROR:
            color = color_orange;
            break;
#endif
          case PANIC:
            color = color_red;
            break;
        }
        strip.setPixelColor(i, color);
      }
      strip.show();
#endif

      while (Serial.available() > 0) {
        int b = Serial.read();
        Serial.print(F("Got "));
        Serial.print(b);
        Serial.write('\n');
        switch (b) {
          case '@':
            for (uint8_t i = 0; i < NUM_MODULES; i++) {
              modules[i].GoHome();
            }
            break;
          case '=':
            recv_count = 0;
            break;
          case '\n':
              Serial.print(F("Going to '"));
              for (uint8_t i = 0; i < NUM_MODULES; i++) {
                int8_t index = FindFlapIndex(recv_buffer[i]);
                if (index != -1) {
                  modules[i].GoToFlapIndex(index);
                }
                Serial.write(recv_buffer[i]);
              }
              Serial.print("'\n");
              break;
          default:
            if (recv_count > NUM_MODULES - 1) {
              break;
            }
            recv_buffer[recv_count] = b;
            recv_count++;
            break;
        }
      }
  
      if (!was_stopped && all_stopped) {
        for (uint8_t i = 0; i < NUM_MODULES; i++) {
          Serial.print(F("---\nStats "));
          Serial.print(i);
          Serial.print(F(":\nM: "));
          Serial.print(modules[i].count_missed_home);
          Serial.print(F("\nU: "));
          Serial.print(modules[i].count_unexpected_home);
          Serial.print('\n');
        }
        Serial.print(F("##########\n"));
      }
      Serial.flush();
    }
    was_idle = all_idle;
    was_stopped = all_stopped;
  }
}
