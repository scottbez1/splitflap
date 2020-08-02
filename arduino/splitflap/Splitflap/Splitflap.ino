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

#include <Arduino.h>
#include <Wire.h>


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
*/

#if NUM_MODULES < 1
#error NUM_MODULES must be at least 1
#endif

#include "splitflap_module.h"

#if SPI_IO
#include "spi_io_config.h"
#else
#include "basic_io_config.h"
#endif

#if NEOPIXEL_DEBUGGING_ENABLED
#include <Adafruit_NeoPixel.h>
#endif

#if SSD1306_DISPLAY
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans12pt7b.h>
#endif

#if INA219_POWER_SENSE
#include "Adafruit_INA219.h"
#endif

#ifdef __AVR__
#define FAVR(x) F(x)
#else
#define FAVR(x) x
#endif

int recv_buffer[NUM_MODULES];

#if NEOPIXEL_DEBUGGING_ENABLED
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_MODULES, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
uint32_t color_green = strip.Color(0, 30, 0);
uint32_t color_red = strip.Color(100, 0, 0);
uint32_t color_purple = strip.Color(15, 0, 15);
uint32_t color_orange = strip.Color(30, 7, 0);
#endif

#if SSD1306_DISPLAY
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
char labelString[NUM_MODULES + 1];
char statusString[NUM_MODULES + 1];
char displayBuffer[200];
char debugMessage[100];
#endif

#if INA219_POWER_SENSE
Adafruit_INA219 powerSense;
// Latest current reading
float currentmA;
uint32_t lastCurrentReadMillis = 0;

// Char buffers for building voltage/current strings
char voltageBuf[10];
char currentBuf[10];
#endif

#if defined(ESP32)
#define MONITOR_SPEED 921600
#else
#define MONITOR_SPEED 38400
#endif

void dump_status(void);

void setup() {
  Serial.begin(MONITOR_SPEED);

  initialize_modules();
  motor_sensor_io();

#ifdef OUTPUT_ENABLE_PIN
  // Wait until after 1 motor_sensor_io to turn on shift register outputs
  pinMode(OUTPUT_ENABLE_PIN, OUTPUT);
  digitalWrite(OUTPUT_ENABLE_PIN, LOW);
#endif

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

#if SSD1306_DISPLAY
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.setTextWrap(false);
  display.setTextColor(SSD1306_WHITE);
  display_large_text("Splitflap");
  delay(500);
  display_large_text("calibrating");

  debugMessage[0] = '\0';
  for (uint8_t i = 0; i < NUM_MODULES; i++) {
    labelString[i] = 'A' + i;
  }
  labelString[NUM_MODULES] = '\0';
#endif

#if !NEOPIXEL_DEBUGGING_ENABLED && !SSD1306_DISPLAY
  pinMode(LED_BUILTIN, OUTPUT);

  // Pulse the builtin LED - not as fun but indicates that we're running
  for (int i = 0; i < 11; i++) {
    digitalWrite(LED_BUILTIN, i % 2 ? HIGH : LOW);
    delay(100);
  }
#endif

  Serial.print("\n\n\n");
  Serial.print(FAVR("{\"type\":\"init\", \"num_modules\":"));
  Serial.print(NUM_MODULES);
  Serial.print(FAVR("}\n"));

  for (uint8_t i = 0; i < NUM_MODULES; i++) {
    recv_buffer[i] = 0;
    modules[i]->Init();
#if !SENSOR_TEST
    modules[i]->GoHome();
#endif
  }

#if INA219_POWER_SENSE
  powerSense.begin();
  Wire.setClock(400000);
#endif
}


inline int8_t FindFlapIndex(uint8_t character) {
    for (int8_t i = 0; i < NUM_FLAPS; i++) {
        if (character == flaps[i]) {
          return i;
        }
    }
    return -1;
}

bool pending_move_response = true;
bool pending_no_op = false;
uint8_t recv_count = 0;

bool disabled = false;

void disableAll(char* message) {
  for (uint8_t i = 0; i < NUM_MODULES; i++) {
    modules[i]->Disable();
  }
  motor_sensor_io();

  if (disabled) {
    return;
  }
  disabled = true;

  Serial.println("#### DISABLED! ####");
  Serial.println(message);
#if SSD1306_DISPLAY
  snprintf(debugMessage, sizeof(debugMessage), "### DISABLED! ###\n%s", message);
#endif
}

boolean was_stopped = false;
uint32_t stopped_at_millis = 0;

inline void run_iteration() {
    uint32_t iterationStartMillis = millis();
    boolean all_idle = true;
    boolean all_stopped = true;
    for (uint8_t i = 0; i < NUM_MODULES; i++) {
      modules[i]->Update();
      bool is_idle = modules[i]->state == PANIC
        || modules[i]->state == STATE_DISABLED
#if HOME_CALIBRATION_ENABLED
        || modules[i]->state == LOOK_FOR_HOME
        || modules[i]->state == SENSOR_ERROR
#endif
        || (modules[i]->state == NORMAL && modules[i]->current_accel_step == 0);

      bool is_stopped = modules[i]->state == PANIC
        || modules[i]->state == STATE_DISABLED
        || modules[i]->current_accel_step == 0;

      all_idle &= is_idle;
      all_stopped &= is_stopped;
      // if (i & 0b11) motor_sensor_io();
    }
    if (all_stopped && !was_stopped) {
      stopped_at_millis = iterationStartMillis;
    }
    was_stopped = all_stopped;
    motor_sensor_io();

#if INA219_POWER_SENSE
    if (iterationStartMillis - lastCurrentReadMillis > 100) {
      currentmA = powerSense.getCurrent_mA();
      if (currentmA > NUM_MODULES * 250) {
        disableAll("Over current");
      } else if (all_stopped && iterationStartMillis - stopped_at_millis > 100 && currentmA >= 3) {
        disableAll("Unexpected current");
      }
      lastCurrentReadMillis = iterationStartMillis;
    }
#endif

    if (all_idle) {
#if NEOPIXEL_DEBUGGING_ENABLED
      for (int i = 0; i < NUM_MODULES; i++) {
        uint32_t color;
        switch (modules[i]->state) {
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

#if SSD1306_DISPLAY
      if (all_stopped) {
        for (int i = 0; i < NUM_MODULES; i++) {
          uint32_t color;
          switch (modules[i]->state) {
            case NORMAL:
              statusString[i] = '_';
              break;
  #if HOME_CALIBRATION_ENABLED
            case LOOK_FOR_HOME:
              statusString[i] = 'H';
              break;
            case SENSOR_ERROR:
              statusString[i] = 'E';
              break;
  #endif
            case STATE_DISABLED:
              statusString[i] = 'D';
              break;
            case PANIC:
              statusString[i] = '!';
              break;
            default:
              statusString[i] = '?';
              break;
          }
        }
        statusString[NUM_MODULES] = '\0';

        display.clearDisplay();
        if (!disabled) {
          display.setCursor(0, 0);
          display.println(labelString);
          display.println(statusString);
        }

#if INA219_POWER_SENSE
        float voltage = powerSense.getBusVoltage_V();
        if (voltage > 14) {
          disableAll("Over voltage");
        } else if (voltage < 10) {
          disableAll("Under voltage");
        }
        dtostrf(voltage, 6, 3, voltageBuf);
        snprintf(displayBuffer, sizeof(displayBuffer), "%sV", voltageBuf);
        display.setCursor(84, 0);
        display.print(displayBuffer);

        dtostrf(currentmA / 1000, 6, 3, currentBuf);
        snprintf(displayBuffer, sizeof(displayBuffer), "%sA", currentBuf);
        display.setCursor(84, 8);
        display.print(displayBuffer);
#endif

        display.setCursor(0, 16);
        display.print(debugMessage);

        if ((millis() >> 7) & 1) {
          display.drawPixel(127, 31, SSD1306_WHITE);
        }

        display.display();
      }
#endif

      if (pending_no_op && all_stopped) {
        Serial.print(FAVR("{\"type\":\"no_op\"}\n"));
        Serial.flush();
        pending_no_op = false;
      }
      if (pending_move_response && all_stopped) {
        pending_move_response = false;
        dump_status();
      }

      while (Serial.available() > 0) {
        int b = Serial.read();
        switch (b) {
          case '@':
#if SSD1306_DISPLAY
              if (all_stopped) {
                display_large_text("calibrating");
              }
#endif
            for (uint8_t i = 0; i < NUM_MODULES; i++) {
              modules[i]->ResetErrorCounters();
              modules[i]->GoHome();
            }
            break;
          case '#':
            pending_no_op = true;
            break;
          case '=':
            recv_count = 0;
            break;
          case '\n':
              pending_move_response = true;
              Serial.print(FAVR("{\"type\":\"move_echo\", \"dest\":\""));
              Serial.flush();
#if SSD1306_DISPLAY
              if (all_stopped) {
                display_large_text("moving...");
              }
#endif
              for (uint8_t i = 0; i < recv_count; i++) {
                int8_t index = FindFlapIndex(recv_buffer[i]);
                if (index != -1) {
                  if (FORCE_FULL_ROTATION || index != modules[i]->GetTargetFlapIndex()) {
                    modules[i]->GoToFlapIndex(index);
                  }
                }
                Serial.write(recv_buffer[i]);
                if (i % 8 == 0) {
                  Serial.flush();
                }
              }
              Serial.print(FAVR("\"}\n"));
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
    }
}

void sensor_test_iteration() {
    motor_sensor_io();

    Serial.println();
#if NEOPIXEL_DEBUGGING_ENABLED
    for (uint8_t i = 0; i < NUM_MODULES; i++) {
      uint32_t color;
      if (!modules[i]->GetHomeState()) {
        color = color_green;
      } else {
        color = color_purple;
      }
      Serial.print(modules[i]->GetHomeState() ? '0' : '1');

      // Make LEDs flash in sequence to indicate sensor test mode
      if ((millis() / 32) % NUM_MODULES == i) {
        color += 8 + (8 << 8) + (8 << 16);
      }
      strip.setPixelColor(i, color);
    }
    strip.show();
#else
#if NUM_MODULES > 1 && SENSOR_TEST
#error NEOPIXEL_DEBUGGING_ENABLED is false, but NUM_MODULES is > 1. To run a sensor test without neopixels, the Arduino will use the builtin LED so NUM_MODULES must be set to 1.
#endif
    // We only have one LED - just show the first module's home state status
    digitalWrite(LED_BUILTIN, !modules[0]->GetHomeState() ? HIGH : LOW);
#endif
delay(100);
}


void loop() {
  while (1) {
#if SENSOR_TEST
    sensor_test_iteration();
#else
    run_iteration();
#endif

    #if defined(ESP8266) || defined(ESP32)
    // Yield to avoid triggering Soft WDT
    yield();
    #endif
  }
}

void dump_status() {
  Serial.print(FAVR("{\"type\":\"status\", \"modules\":["));
  for (uint8_t i = 0; i < NUM_MODULES; i++) {
    Serial.print(FAVR("{\"state\":\""));
    switch (modules[i]->state) {
      case NORMAL:
        Serial.print(FAVR("normal"));
        break;
#if HOME_CALIBRATION_ENABLED
      case LOOK_FOR_HOME:
        Serial.print(FAVR("look_for_home"));
        break;
      case SENSOR_ERROR:
        Serial.print(FAVR("sensor_error"));
        break;
#endif
      case PANIC:
        Serial.print(FAVR("panic"));
        break;
    }
    Serial.print(FAVR("\", \"flap\":\""));
    Serial.write(flaps[modules[i]->GetCurrentFlapIndex()]);
    Serial.print(FAVR("\", \"count_missed_home\":"));
    Serial.print(modules[i]->count_missed_home);
    Serial.print(FAVR(", \"count_unexpected_home\":"));
    Serial.print(modules[i]->count_unexpected_home);
    Serial.print(FAVR("}"));
    if (i < NUM_MODULES - 1) {
      Serial.print(FAVR(", "));
    }
  }
  Serial.print(FAVR("]}\n"));
  Serial.flush();
}

#if SSD1306_DISPLAY
void display_large_text(char* message) {
  display.clearDisplay();
  display.setFont(&FreeSans12pt7b);

  int16_t outX, outY;
  uint16_t outW, outH;
  display.getTextBounds(message, 0, 0, &outX, &outY, &outW, &outH);

  display.setCursor(0, outH);
  display.print(message);
  display.display();

  // Reset text size
  display.setFont();
}
#endif
