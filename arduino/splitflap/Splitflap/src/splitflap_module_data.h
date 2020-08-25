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
#pragma once

#include "../config.h"

#if HOME_CALIBRATION_ENABLED
enum HomeState {
    // Ignore any home blips (e.g. if we've just seen the home position and haven't traveled past it yet)
    IGNORE,
    // Home isn't expected; a home blip in this state/region indicates an error that requires recalibration
    UNEXPECTED,
    // Home position is expected in this state/region
    EXPECTED,
};
#endif

enum State {
  NORMAL,
  PANIC,
  STATE_DISABLED,
  LOOK_FOR_HOME,
  SENSOR_ERROR,
};
