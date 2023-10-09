#pragma once

#include <Arduino.h>

/***** CONFIGURATION *****/

// 1) Mode
#define SENSOR_TEST false

// 2) General Settings
#ifndef NUM_MODULES
#define NUM_MODULES (12)
#endif

// Whether to force a full rotation when the same letter is specified again
#define FORCE_FULL_ROTATION true

// Whether to use/expect a home sensor. Enable for auto-calibration via home
// sensor feedback. Disable for basic open-loop control (useful when first
// testing the split-flap, since home calibration can be tricky to fine tune)
#define HOME_CALIBRATION_ENABLED true

// 3) Flap Contents & Order
#define NUM_FLAPS (40)

// This should match the order of flaps on the spool, with the first being the
// "home" flap.
const uint8_t flaps[NUM_FLAPS] = {
  ' ',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  '.',
  ',',
  '\'',
};

// 4) Hardware configuration and features
#ifndef SPLITFLAP_PIO_HARDWARE_CONFIG
  // Note: these values are only used in the Arduino IDE. For PlatformIO,
  // hardware configuration is set by the environment, in platformio.ini.
  #define SPI_IO true
  #define REVERSE_MOTOR_DIRECTION false
  #define NEOPIXEL_DEBUGGING_ENABLED true
  #define SSD1306_DISPLAY false
  #define INA219_POWER_SENSE false
#endif

// 5) Board-dependent Default Settings

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
  #if SPI_IO
  #define NEOPIXEL_PIN 6
  #else
  #define NEOPIXEL_PIN 3
  #endif
  #define MONITOR_SPEED 38400
#elif defined(__AVR_ATmega2560__)
  #define NEOPIXEL_PIN 5
  #define MONITOR_SPEED 38400
#elif defined(ARDUINO_ESP8266_WEMOS_D1MINI)
  #define NEOPIXEL_PIN (D8)
  #define MONITOR_SPEED 38400
#elif defined(ESP32)
  #define NEOPIXEL_PIN (13)
  #define MONITOR_SPEED 230400
#endif

/*************************/


/**
 * Suggested Pinouts
 *
 * Arduino Uno - Basic IO - Up to 3 modules
 *    A: Motor: B0-3 = pins 8-11        Sensor: B4 = pin 12
 *    B: Motor: D4-7 = pins 4-7         Sensor: C4 = pin A4
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
 *     25 Latch         (out)
 *     33 CLK           (out)
 *     39 MISO          (in)
 *     32 MOSI          (out)
 *
 */


// Configuration validation and dependent settings
#if NUM_MODULES < 1
#error NUM_MODULES must be at least 1
#endif
#ifdef CHAINLINK
#if NUM_MODULES % 6 != 0
#error NUM_MODULES must be a multiple of 6, to match the capacity of the connected Chainlink Drivers. You can use fewer than this number of modules, but the setting must match the Chainlink Driver outputs.
#endif

#define NUM_LOOPBACKS (NUM_MODULES / 3)
#define CHAINLINK_ENFORCE_LOOPBACKS 1
#endif
