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

#include "display_task.h"
#include "splitflap_task.h"

SplitflapTask splitflapTask(1);
DisplayTask displayTask(splitflapTask, 0);

#ifdef MQTT
#include "mqtt_task.h"
MQTTTask mqttTask(splitflapTask, 0);
#endif

void setup() {
  Serial.begin(MONITOR_SPEED);

  splitflapTask.begin();

  #ifdef MQTT
  mqttTask.begin();
  #endif
  displayTask.begin();

  // Delete the default Arduino loopTask to free up Core 1
  vTaskDelete(NULL);
}


void loop() {
  assert(false);
}
