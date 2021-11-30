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
#include <TFT_eSPI.h>
#include <Wire.h>

#include "config.h"

#include "../core/splitflap_task.h"
#include "display_task.h"
#include "serial_task.h"

SplitflapTask splitflapTask(1, LedMode::AUTO);
SerialTask serialTask(splitflapTask, 0);

#if MQTT
#include "mqtt_task.h"
MQTTTask mqttTask(splitflapTask, serialTask, 0);
#endif

#if HTTP
#include "http_task.h"
HTTPTask httpTask(splitflapTask, serialTask, 0);
#endif

#if ENABLE_DISPLAY
DisplayTask displayTask(splitflapTask, 0);
#endif

#ifdef CHAINLINK_BASE
#include "../base/base_supervisor_task.h"
BaseSupervisorTask baseSupervisorTask(splitflapTask, serialTask, 0);
#endif

void setup() {
  char buf[200];
  serialTask.begin();

  snprintf(buf, sizeof(buf), "1 Free heap: %u, block: %u", ESP.getFreeHeap(), ESP.getMaxAllocHeap());
  serialTask.log(buf);

  splitflapTask.begin();

  snprintf(buf, sizeof(buf), "2 Free heap: %u, block: %u", ESP.getFreeHeap(), ESP.getMaxAllocHeap());
  serialTask.log(buf);

  #if ENABLE_DISPLAY
  displayTask.begin();
  #endif

  snprintf(buf, sizeof(buf), "3 Free heap: %u, block: %u", ESP.getFreeHeap(), ESP.getMaxAllocHeap());
  serialTask.log(buf);

  delay(5000);

  snprintf(buf, sizeof(buf), "4 Free heap: %u, block: %u", ESP.getFreeHeap(), ESP.getMaxAllocHeap());
  serialTask.log(buf);

  #if MQTT
  mqttTask.begin();
  #endif


  #if HTTP
  httpTask.begin();
  #endif


  #ifdef CHAINLINK_BASE
  baseSupervisorTask.begin();
  #endif


  // Delete the default Arduino loopTask to free up Core 1
  vTaskDelete(NULL);
}


void loop() {
  assert(false);
}
