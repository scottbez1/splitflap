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

#include <Arduino.h>

#include "splitflap_module.h"

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
  #if NUM_MODULES > 3
  #error "Basic IO mode only supports up to 3 modules on Atmega168/328-based boards. Set NUM_MODULES to 3 or fewer."
  #endif
  // Motor A: PB0-3 = pins 8-11
  // Motor B: PD4-7 = pins 4-7
  // Motor C: PC0-3 = pins A0-A3

  // Sensor A: PB4 = pin 12
  // Sensor B: PC4 = pin A4
  // Sensor C: PC5 = pin A5

  SplitflapModule moduleA((uint8_t&)PORTB, 0, (uint8_t&)PINB, B00010000);
  SplitflapModule moduleB((uint8_t&)PORTD, 4, (uint8_t&)PINC, B00010000);
  SplitflapModule moduleC((uint8_t&)PORTC, 0, (uint8_t&)PINC, B00100000);

  SplitflapModule* modules[] = {
    &moduleA,
    &moduleB,
    &moduleC,
  };

  void initialize_modules() {
    // Initialize motor outputs
    DDRB |= 0xF; // Motor A
    DDRD |= 0xF0; // Motor B
    DDRC |= 0xF; // Motor C

    // Initialize sensor inputs with pullups
    pinMode(12, INPUT_PULLUP); // Sensor A
    pinMode(18, INPUT_PULLUP); // Sensor B
    pinMode(19, INPUT_PULLUP); // Sensor C
  }

  inline void motor_sensor_io() {
    // No-op (modules write directly to IO pins)
  }
#elif defined(__AVR_ATmega2560__)

  SplitflapModule moduleA((uint8_t&)PORTB, 4, (uint8_t&)PINE, 1 << 5); //10-13    3
  SplitflapModule moduleB((uint8_t&)PORTA, 0, (uint8_t&)PINE, 1 << 4); //25-22    2
  SplitflapModule moduleC((uint8_t&)PORTA, 4, (uint8_t&)PINJ, 1 << 1); //29-26    14
  SplitflapModule moduleD((uint8_t&)PORTC, 4, (uint8_t&)PINJ, 1 << 0); //33-30    15
  SplitflapModule moduleE((uint8_t&)PORTC, 0, (uint8_t&)PINH, 1 << 1); //37-34    16
  SplitflapModule moduleF((uint8_t&)PORTL, 4, (uint8_t&)PINH, 1 << 0); //45-42    17
  SplitflapModule moduleG((uint8_t&)PORTL, 0, (uint8_t&)PIND, 1 << 3); //49-46    18
  SplitflapModule moduleH((uint8_t&)PORTB, 0, (uint8_t&)PIND, 1 << 2); //53-50    19
  SplitflapModule moduleI((uint8_t&)PORTK, 4, (uint8_t&)PIND, 1 << 7); //A12-A15  38
  SplitflapModule moduleJ((uint8_t&)PORTK, 0, (uint8_t&)PING, 1 << 2); //A8-A11   39
  SplitflapModule moduleK((uint8_t&)PORTF, 4, (uint8_t&)PING, 1 << 1); //A4-A7    40
  SplitflapModule moduleL((uint8_t&)PORTF, 0, (uint8_t&)PING, 1 << 0); //A0-A3    41

  SplitflapModule* modules[] = {
    &moduleA,
    &moduleB,
    &moduleC,
    &moduleD,
    &moduleE,
    &moduleF,
    &moduleG,
    &moduleH,
    &moduleI,
    &moduleJ,
    &moduleK,
    &moduleL,
  };

  void initialize_modules() {
    // Initialize motor outputs
    DDRF = 0xFF;
    DDRK = 0xFF;
    DDRB = 0xFF;
    DDRL = 0xFF;
    DDRC = 0xFF;
    DDRA = 0xFF;

    // Initialize sensor inputs with pullups
    pinMode(41, INPUT_PULLUP);
    pinMode(40, INPUT_PULLUP);
    pinMode(39, INPUT_PULLUP);
    pinMode(38, INPUT_PULLUP);
    pinMode(19, INPUT_PULLUP);
    pinMode(18, INPUT_PULLUP);
    pinMode(17, INPUT_PULLUP);
    pinMode(16, INPUT_PULLUP);
    pinMode(15, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP);
    pinMode(2, INPUT_PULLUP);
    pinMode(3, INPUT_PULLUP);
  }

  inline void motor_sensor_io() {
    // No-op (modules write directly to IO pins)
  }
#else
  #error "Basic IO configuration is not supported for this board type. Use SPI IO or modify basic_io_config.h to add support for this board."
#endif

#endif
