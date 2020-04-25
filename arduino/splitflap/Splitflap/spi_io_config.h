/*
   Copyright 2020 Scott Bezek and the splitflap contributors

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

#include <SPI.h>

#define NEOPIXEL_DEBUGGING_ENABLED true

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
  #define NEOPIXEL_PIN 6

  #define OUT_LATCH_PIN (4)
  #define _OUT_LATCH_PORT PORTD
  #define _OUT_LATCH_BIT 4

  #define IN_LATCH_PIN (5)
  #define _IN_LATCH_PORT PORTD
  #define _IN_LATCH_BIT 5

  #define OUT_LATCH(){\
    _OUT_LATCH_PORT |= (1 << _OUT_LATCH_BIT);\
    _OUT_LATCH_PORT &= ~(1 << _OUT_LATCH_BIT);\
  }
  #define IN_LATCH() {\
    _IN_LATCH_PORT &= ~(1 << _IN_LATCH_BIT);\
    _IN_LATCH_PORT |= (1 << _IN_LATCH_BIT);\
  }
#endif
#ifdef ARDUINO_ESP8266_WEMOS_D1MINI
  #define NEOPIXEL_PIN (D8)
  #define OUT_LATCH_PIN (D1)
  #define IN_LATCH_PIN (D2)

  #define OUT_LATCH() {digitalWrite(OUT_LATCH_PIN, HIGH); digitalWrite(OUT_LATCH_PIN, LOW);}
  #define IN_LATCH() {digitalWrite(IN_LATCH_PIN, LOW); digitalWrite(IN_LATCH_PIN, HIGH);}
#endif

#ifdef ESP32
  #define NEOPIXEL_PIN (13) //Any pin will work
  #define OUT_LATCH_PIN (12)//Any pin will work
  #define IN_LATCH_PIN (27) //Any pin will work

  #define OUT_LATCH() {digitalWrite(OUT_LATCH_PIN, HIGH); digitalWrite(OUT_LATCH_PIN, LOW);}
  #define IN_LATCH() {digitalWrite(IN_LATCH_PIN, LOW);digitalWrite(IN_LATCH_PIN, LOW);digitalWrite(IN_LATCH_PIN, HIGH);}  //on ESP32, for some weird reason, it sometimes doesnt work if we do only 1 digitalWrite!
#endif

#if !defined(OUT_LATCH) || !defined(IN_LATCH)
#error "Unknown/unsupported board. ATmega328-based boards (Uno, Duemilanove, Diecimila), ESP8266 and ESP32 are currently supported"
#endif


#define MOTOR_BUFFER_LENGTH (NUM_MODULES / 2 + (NUM_MODULES % 2 != 0))
uint8_t motor_buffer[MOTOR_BUFFER_LENGTH];

#define SENSOR_BUFFER_LENGTH (NUM_MODULES / 4 + (NUM_MODULES % 4 != 0))
uint8_t sensor_buffer[SENSOR_BUFFER_LENGTH];

// Define placement new so we can initialize SplitflapModules at runtime into a static buffer.
// (see https://arduino.stackexchange.com/a/1499)
void* operator new(size_t size, void* ptr) {
  return ptr;
}

// Static buffer for SplitflapModules (initialized at runtime)
static char moduleBuffer[NUM_MODULES][sizeof(SplitflapModule)];

SplitflapModule* modules[NUM_MODULES];

inline void initialize_modules() {
  for (uint8_t i = 0; i < NUM_MODULES; i++) {
    // Create SplitflapModules in a statically allocated buffer using placement new
    modules[i] = new (moduleBuffer[i]) SplitflapModule(motor_buffer[i/2], i % 2 == 0 ? 0 : 4, sensor_buffer[i/4], 1 << (i % 4));
  }

  // Initialize SPI
  pinMode(IN_LATCH_PIN, OUTPUT);
  pinMode(OUT_LATCH_PIN, OUTPUT);
  digitalWrite(IN_LATCH_PIN, HIGH);

  //SPI.begin(18, 19, 23, 27);  //-->ESP32 pins VSPI
  SPI.begin();  //ESP32 uses the VSPI pins by default
  SPI.beginTransaction(SPISettings(3000000, MSBFIRST, SPI_MODE0));
  
  for (uint8_t i = 0; i < MOTOR_BUFFER_LENGTH; i++) {
    motor_buffer[i] = 0;
  }
  for (uint8_t i = 0; i < SENSOR_BUFFER_LENGTH; i++) {
    sensor_buffer[i] = 0;
  }
}

inline void motor_sensor_io() {
  IN_LATCH();

#ifdef ARDUINO_ESP8266_WEMOS_D1MINI
  // Due to timing differences in the SPI implementation on the ESP8266, the
  // first bit is "lost" when using SPI to shift in data from a 74HC165
  // register. To correct for that, we manually read the first bit before
  // using SPI to read byte-by-byte and shift the data accordingly.
  bool extra_bit = digitalRead(D6);
#elif defined(ESP32)
  // Same problem for the ESP32, same solution.
  bool extra_bit = digitalRead(MISO);
#endif
  for (uint8_t i = 0; i < MOTOR_BUFFER_LENGTH; i++) {
    int val = SPI.transfer(motor_buffer[MOTOR_BUFFER_LENGTH - 1 - i]);
    if (i < SENSOR_BUFFER_LENGTH) {
#if defined(ARDUINO_ESP8266_WEMOS_D1MINI) || defined(ESP32)
      sensor_buffer[i] = (extra_bit << 7) | (val >> 1);
      extra_bit = val & B00000001;
#else
      sensor_buffer[i] = val;
#endif
    }
  }

  OUT_LATCH();
}

#endif
