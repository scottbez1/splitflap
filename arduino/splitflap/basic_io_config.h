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

#if NUM_MODULES > 3
#error "Basic IO mode only supports up to 3 modules. Set NUM_MODULES to 3 or fewer."
#endif

// Neopixel debugging isn't currently supported with basic IO mode
#define NEOPIXEL_DEBUGGING_ENABLED false

#if !defined(__AVR_ATmega168__) && !defined(__AVR_ATmega328P__)
#error "Basic IO configuration only works for ATmega168/328-based boards (Arduino Diecimila, Duemilanove, Uno)"  
#endif

// Pinout:
// Motor A: PD4-7 = pins 4-7
// Motor B: PB0-3 = pins 8-11
// Motor C: PC0-3 = pins A0-A3

// Sensor A: PB4 = pin 12
// Sensor B: PC4 = pin A4
// Sensor C: PC5 = pin A5

SplitflapModule moduleA((uint8_t&)PORTD, 4, (uint8_t&)PINB, B00010000);
SplitflapModule moduleB((uint8_t&)PORTB, 0, (uint8_t&)PINC, B00010000);
SplitflapModule moduleC((uint8_t&)PORTC, 0, (uint8_t&)PINC, B00100000);

SplitflapModule modules[] = {
  moduleA,
  moduleB,
  moduleC,
};

inline void motor_sensor_setup() {
  // Initialize motor outputs
  DDRD |= 0xF0; // Motor A
  DDRB |= 0xF; // Motor B
  DDRC |= 0xF; // Motor C

  // Initialize sensor inputs with pullups
  pinMode(12, INPUT_PULLUP); // Sensor A
  pinMode(18, INPUT_PULLUP); // Sensor B
  pinMode(19, INPUT_PULLUP); // Sensor C
}

inline void motor_sensor_io() {
  // No-op (modules write directly to IO pins)
}

#endif
