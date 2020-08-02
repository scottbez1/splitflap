#pragma once

#include <Arduino.h>

/***** CONFIGURATION *****/

// 1) Mode
#define SENSOR_TEST false

// 2) Basic Settings
#define NUM_MODULES (12)
#define SPI_IO false
#define REVERSE_MOTOR_DIRECTION false

// Whether to force a full rotation when the same letter is specified again
#define FORCE_FULL_ROTATION true

// 3) Optional Features
#define NEOPIXEL_DEBUGGING_ENABLED false
#define SSD1306_DISPLAY true
#define INA219_POWER_SENSE true

// 4) Flap Contents & Order

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

// 5) Board-dependent Default Settings

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
  #if SPI_IO
  #define NEOPIXEL_PIN 6
  #else
  #define NEOPIXEL_PIN 3
  #endif
#elif defined(__AVR_ATmega2560__)
  #define NEOPIXEL_PIN 4
#elif defined(ARDUINO_ESP8266_WEMOS_D1MINI)
  #define NEOPIXEL_PIN (D8)
#elif defined(ESP32)
  #define NEOPIXEL_PIN (13)
#endif

/*************************/


/**
 * Suggested Pinouts
 *
 * Arduino Uno - Basic IO - Up to 3 modules
 *    A: Motor: D4-7 = pins 4-7         Sensor: B4 = pin 12
 *    B: Motor: B0-3 = pins 8-11        Sensor: C4 = pin A4
 *    C: Motor: C0-3 = pins A0-A3       Sensor: C5 = pin A5
 *
 *    Neopixel: 3
 *
 * Arduino Uno - SPI IO (Shift Register) - Up to 12 modules
 *    4   Motor latch
 *    5   Sensor latch
 *    6   neopixel
 *    11  MOSI
 *    12  MISO
 *    13  CLK
 *
 * Arduino Mega 2560 - Basic IO - Up to 12 modules)
 *    Note the order of pins increases for some motors and decreases for others.
 *    Modules whose motor phase pins are in the opposite order are marked with a * below.
 *
 *    A: Motor: F0-3 = pins A0-A3       Sensor: G0 = pin 41
 *    B: Motor: F4-7 = pins A4-A7       Sensor: G1 = pin 40
 *    C: Motor: K0-3 = pins A8-A11      Sensor: G2 = pin 39
 *    D: Motor: K4-7 = pins A12-A15     Sensor: D7 = pin 38
 *    E: Motor: B0-3 = pins 53-50 *     Sensor: D2 = pin 19
 *    F: Motor: L0-3 = pins 49-46 *     Sensor: D3 = pin 18
 *    G: Motor: L4-7 = pins 45-42 *     Sensor: H0 = pin 17
 *    H: Motor: C0-3 = pins 37-34 *     Sensor: H1 = pin 16
 *    I: Motor: C4-7 = pins 33-30 *     Sensor: J0 = pin 15
 *    J: Motor: A4-7 = pins 29-26 *     Sensor: J1 = pin 14
 *    K: Motor: A0-3 = pins 25-22 *     Sensor: E4 = pin 2
 *    L: Motor: B4-7 = pins 10-13       Sensor: E5 = pin 3
 *
 *    Neopixel: 4
 *
 *    INA219 & SSD1306 Oled:
 *      SDA: 20
 *      SCL: 21
 *
 * ESP32 - SPI IO (Shift Register) - Up to 120+ modules:
 *     5  CS            (out) (debug only)
 *     12 Motor latch   (out)
 *     13 neopixel      (out)
 *     18 CLK           (out)
 *     19 MISO          (in)
 *     23 MOSI          (out)
 *     27 Sensor latch  (out)
 *     32 Output enable (out)
 *
 */
