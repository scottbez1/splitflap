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

#include "config.h"

#include "../core/splitflap_task.h"
#include "tester_task.h"

SplitflapTask splitflapTask(1, LedMode::MANUAL);
TesterTask testerTask(splitflapTask, 0);

void setup() {
  Serial.begin(MONITOR_SPEED);

  splitflapTask.begin();
  testerTask.begin();

  // Delete the default Arduino loopTask to free up Core 1
  vTaskDelete(NULL);
}


void loop() {
  assert(false);
}
