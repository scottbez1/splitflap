/*
   Copyright 2021 Scott Bezek and the splitflap contributors

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
#pragma once

#define NUM_POWER_CHANNELS      5
#define NUM_LEDS (NUM_POWER_CHANNELS + 1)

#define BASE_NEOPIXEL_PIN       27
#define BASE_MCP_NRESET_PIN     26
#define BASE_MASTER_EN_PIN      17

#define BASE_MCP_ADDRESS        7

#define MCP_PIN_CHANNEL_0_EN    5
#define MCP_PIN_CHANNEL_1_EN    4
#define MCP_PIN_CHANNEL_2_EN    3
#define MCP_PIN_CHANNEL_3_EN    2
#define MCP_PIN_CHANNEL_4_EN    1
#define MCP_PIN_ADDR_1          15
#define MCP_PIN_ADDR_2          14
#define MCP_PIN_ADDR_4          13
#define MCP_PIN_ADDR_8          12

#define ABSOLUTE_MAX_VOLTAGE 15
#define ABSOLUTE_MAX_CHANNEL_CURRENT_MA  9000 // Absolute maximum current allowable (actual limits will be lower and adjust dynamically based on load)

#define MIN_RUN_VOLTAGE 11.5
#define MAX_DISABLED_VOLTAGE 3

#define IDLE_CURRENT_MILLIAMPS  25  // Max acceptable power channel idle current. Should account for measurement error and channel Out LED (if enabled).

#define MIN_MODULE_CURRENT_MA           100
#define MAX_MODULE_CURRENT_HOMING_MA    260
#define MAX_MODULE_CURRENT_MOVING_MA    260 //180

#define CONSECUTIVE_CURRENT_OUT_OF_RANGE_THRESHOLD  20
#define CONSECUTIVE_UNEXPECTED_POWER_THRESHOLD 30