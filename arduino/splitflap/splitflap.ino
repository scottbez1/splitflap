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

/***** CONFIGURATION *****/

#define NUM_MODULES (12)
#define SENSOR_TEST false
#define NEOPIXEL_DEBUGGING_ENABLED true
#define NEOPIXEL_PIN 6
#define SPI_IO true
#define REVERSE_MOTOR_DIRECTION false

// This should match the order of flaps on the spool:
const uint8_t flaps[] = {
  ' ',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  '.',
  ',',
  '\'',
};

/*************************/

#include "splitflap_module.h"

#include <SPI.h>
#if NEOPIXEL_DEBUGGING_ENABLED
#include <Adafruit_NeoPixel.h>
#endif

#if SPI_IO
#include "spi_io_config.h"
#else
#include "basic_io_config.h"
#endif

int recv_buffer[NUM_MODULES];

#if NEOPIXEL_DEBUGGING_ENABLED
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_MODULES, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
uint32_t color_green = strip.Color(0, 30, 0);
uint32_t color_red = strip.Color(100, 0, 0);
uint32_t color_purple = strip.Color(15, 0, 15);
uint32_t color_orange = strip.Color(30, 7, 0);
#endif


void setup() {
  Serial.begin(38400);

  motor_sensor_setup();
  motor_sensor_io();

  Serial.print(F("{\"type\":\"init\", \"num_modules\":"));
  Serial.print(NUM_MODULES);
  Serial.print(F("}\n"));

#if NEOPIXEL_DEBUGGING_ENABLED
  strip.begin();
  strip.show();

  // Pulse neopixels for fun
  uint8_t vals[] = {0, 1, 3, 10, 16, 20, 24, 28, 28, 24, 20, 16, 10, 3, 1, 0};
  for (int16_t i = 0; i < 400; i++) {
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
    delay(3);
  }
#endif

  for (uint8_t i = 0; i < NUM_MODULES; i++) {
    recv_buffer[i] = 0;
    modules[i].Init();
#if !SENSOR_TEST
    modules[i].GoHome();
#endif
  }
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
bool pending_no_op = false;
uint8_t recv_count = 0;

inline void run_iteration() {
    boolean all_idle = true;
    boolean all_stopped = true;
    boolean any_bad_timing = false;
    for (uint8_t i = 0; i < NUM_MODULES; i++) {
      any_bad_timing |= modules[i].Update();
      bool is_idle = modules[i].state == PANIC
#if HOME_CALIBRATION_ENABLED
        || modules[i].state == LOOK_FOR_HOME
        || modules[i].state == SENSOR_ERROR
#endif
        || (modules[i].state == NORMAL && modules[i].current_accel_step == 0);
      all_idle &= is_idle;
      all_stopped &= modules[i].current_accel_step == 0;
      if (i & 0b11) motor_sensor_io();
    }
    motor_sensor_io();

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
        switch (b) {
          case '@':
            for (uint8_t i = 0; i < NUM_MODULES; i++) {
              modules[i].ResetErrorCounters();
              modules[i].GoHome();
            }
            break;
          case '#':
            pending_no_op = true;
            break;
          case '=':
            recv_count = 0;
            break;
          case '\n':
              Serial.print(F("{\"type\":\"move_echo\", \"dest\":\""));
              for (uint8_t i = 0; i < recv_count; i++) {
                int8_t index = FindFlapIndex(recv_buffer[i]);
                if (index != -1) {
                  modules[i].GoToFlapIndex(index);
                }
                Serial.write(recv_buffer[i]);
              }
              Serial.print(F("\"}\n"));
              Serial.flush();
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

      if (pending_no_op && all_stopped) {
        Serial.print(F("{\"type\":\"no_op\"}\n"));
        Serial.flush();
        pending_no_op = false;
      }
  
      if (!was_stopped && all_stopped) {
        dump_status();
      }

    }
    was_idle = all_idle;
    was_stopped = all_stopped;
}

void sensor_test_iteration() {
    motor_sensor_io();
    for (uint8_t i = 0; i < NUM_MODULES; i++) {
      uint32_t color;
      if (!modules[i].GetHomeState()) {
        color = color_green;
      } else {
        color = color_purple;
      }

      // Make LEDs flash in sequence to indicate sensor test mode
      if ((millis() / 32) % NUM_MODULES == i) {
        color += 8 + (8 << 8) + (8 << 16);
      }
      strip.setPixelColor(i, color);
    }
    strip.show();
}


void loop() {
  while (1) {
#if SENSOR_TEST
    sensor_test_iteration();
#else
    run_iteration();
#endif
  }
}

void dump_status() {
  Serial.print(F("{\"type\":\"status\", \"modules\":["));
  for (uint8_t i = 0; i < NUM_MODULES; i++) {
    Serial.print(F("{\"state\":\""));
    switch (modules[i].state) {
      case NORMAL:
        Serial.print(F("normal"));
        break;
#if HOME_CALIBRATION_ENABLED
      case LOOK_FOR_HOME:
        Serial.print(F("look_for_home"));
        break;
      case SENSOR_ERROR:
        Serial.print(F("sensor_error"));
        break;
#endif
      case PANIC:
        Serial.print(F("panic"));
        break;
    }
    Serial.print(F("\", \"flap\":\""));
    Serial.write(flaps[modules[i].GetCurrentFlapIndex()]);
    Serial.print(F("\", \"count_missed_home\":"));
    Serial.print(modules[i].count_missed_home);
    Serial.print(F(", \"count_unexpected_home\":"));
    Serial.print(modules[i].count_unexpected_home);
    Serial.print(F("}"));
    if (i < NUM_MODULES - 1) {
      Serial.print(F(", "));
    }
  }
  Serial.print(F("]}\n"));
  Serial.flush();
}
