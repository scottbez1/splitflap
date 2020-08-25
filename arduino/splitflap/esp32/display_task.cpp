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
#include "display_task.h"

DisplayTask::DisplayTask(SplitflapTask& splitflap_task, const uint8_t task_core) : Task{"Display", 8192, 1, task_core}, splitflap_task_{splitflap_task} {}

void DisplayTask::run() {
    tft_.begin();
    tft_.invertDisplay(1);
    tft_.setRotation(0);

    spr_.setColorDepth(16);
    spr_.createSprite(TFT_WIDTH, TFT_HEIGHT);
    spr_.setFreeFont(&FreeMono9pt7b);
    spr_.setTextColor(0xFFFF, TFT_BLACK);
    while(1) {
        SplitflapState state = splitflap_task_.getState();

        static uint32_t loop_count = 0;
        loop_count++;

        spr_.fillSprite(TFT_BLACK);

        for (uint8_t i = 0; i < NUM_MODULES; i++) {
            SplitflapModuleState& s = state.modules[i];

            spr_.setCursor(0, 15 + i*17);
            char c;
            switch (s.state) {
                case NORMAL:
                    c = flaps[s.flapIndex];
                    break;
                case PANIC:
                    c = '~';
                    break;
                case STATE_DISABLED:
                    c = '*';
                    break;
                case LOOK_FOR_HOME:
                    c = '?';
                    break;
                case SENSOR_ERROR:
                    c = '!';
                    break;
                default:
                    c = ' ';
                    break;
            }
            spr_.printf("%c %c", 'A' + i, c);
        }

        spr_.pushSprite(0, 0);
        delay(10);
    }
}
