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
