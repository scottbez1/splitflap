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

//***** READ ME FIRST *****//
//
// Set up the configuration in config.h
//
// We highly recommend using PlatformIO (via Visual Studio Code) rather than
// the Arduino IDE for this project.
//
// Although PlatformIO may be slightly more complicated to learn at first, it
// pays for itself quickly by making library management and board management
// automatic, and is generally a far more powerful development environment.
//
// While we strive to maintain Arduino IDE support for this project, all active
// development is being done with PlatformIO, so Arduino IDE compatibility may
// be inadvertantly broken from time to time. Please open an issue on Github
// if you run into problems with the Arduino IDE.
//
//*************************//

#include <Arduino.h>
#include <Wire.h>

#include "config.h"
#include "src/splitflap_module.h"

#if SPI_IO
#include "src/spi_io_config.h"
#else
#include "src/basic_io_config.h"
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
#include "src/Adafruit_INA219.h"
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
  Serial.print(FAVR(", \"character_list\":\""));
  for(uint8_t i = 0; i < NUM_FLAPS; i++) {
    Serial.print((char)flaps[i]);
  }
  Serial.print(FAVR("\"}\n"));

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


inline int8_t FindFlapIndex(uint8_t character, uint8_t current_flap_index) {
    for (uint8_t i = current_flap_index; i < NUM_FLAPS; i++) {
        if (character == flaps[i]) {
          return i;
        }
    }
    for (uint8_t i = 0; i < current_flap_index; i++) {
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
        || modules[i]->state == LOOK_FOR_HOME
        || modules[i]->state == SENSOR_ERROR
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
        uint32_t color = 0;
        switch (modules[i]->state) {
          case NORMAL:
            color = color_green;
            break;
          case LOOK_FOR_HOME:
            color = color_purple;
            break;
          case SENSOR_ERROR:
            color = color_orange;
            break;
          case PANIC: // Intentional fall-through
          case STATE_DISABLED:
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
            case LOOK_FOR_HOME:
              statusString[i] = 'H';
              break;
            case SENSOR_ERROR:
              statusString[i] = 'E';
              break;
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
                int8_t index = FindFlapIndex(recv_buffer[i], modules[i]->GetCurrentFlapIndex());
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
        color += 8 + (8 << 8) + ((uint32_t)8 << 16);
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
      case LOOK_FOR_HOME:
        Serial.print(FAVR("look_for_home"));
        break;
      case SENSOR_ERROR:
        Serial.print(FAVR("sensor_error"));
        break;
      case PANIC:
        Serial.print(FAVR("panic"));
        break;
      case STATE_DISABLED:
        Serial.print(FAVR("disabled"));
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
