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
#include "display_layouts.h"
#include "display_task.h"

DisplayTask::DisplayTask(SplitflapTask& splitflap_task, const uint8_t task_core) : Task{"Display", 8192, 1, task_core}, splitflap_task_{splitflap_task} {}

static const int32_t MODULE_WIDTH = 8;
static const int32_t MODULE_HEIGHT = 13;

void DisplayTask::run() {
    tft_.begin();
    tft_.invertDisplay(1);
    tft_.setRotation(1);

    spr_.setColorDepth(16);
    spr_.createSprite(TFT_HEIGHT, TFT_WIDTH);
    spr_.setTextFont(0);
    spr_.setTextColor(0xFFFF, TFT_BLACK);

    uint8_t module_row, module_col;
    int32_t module_x, module_y;
    while(1) {
        SplitflapState state = splitflap_task_.getState();

        static uint32_t loop_count = 0;
        loop_count++;

        spr_.fillSprite(TFT_BLACK);
        spr_.fillRect(MODULE_WIDTH, MODULE_HEIGHT, COLUMNS * (MODULE_WIDTH + 1) + 1, ((NUM_MODULES + COLUMNS - 1) / COLUMNS) * (MODULE_HEIGHT + 1) + 1, 0x2104);

        for (uint8_t i = 0; i < NUM_MODULES; i++) {
            SplitflapModuleState& s = state.modules[i];

            uint16_t background = 0x0000;
            uint16_t foreground = 0xFFFF;

            char c;
            switch (s.state) {
                case NORMAL:
                    c = flaps[s.flap_index];
                    break;
                case PANIC:
                    c = '~';
                    background = 0xD000;
                    break;
                case STATE_DISABLED:
                    c = '*';
                    break;
                case LOOK_FOR_HOME:
                    c = '?';
                    background = 0x6018;
                    break;
                case SENSOR_ERROR:
                    c = ' ';
                    background = 0xD461;
                    foreground = 0x0000;
                    break;
                default:
                    c = ' ';
                    break;
            }
            getLayoutPosition(i, &module_row, &module_col);

            // Leave an empty row and column for headers, and add 1 to width/height as a separator line between modules
            module_x = (module_col + 1) * (MODULE_WIDTH + 1);
            module_y = (module_row + 1) * (MODULE_HEIGHT + 1);

            spr_.setTextColor(foreground, background);
            spr_.fillRect(module_x, module_y, MODULE_WIDTH, MODULE_HEIGHT, background);
            spr_.setCursor(module_x + 1, module_y + 2);
            spr_.printf("%c", c);
        }

        spr_.pushSprite(0, 0);
        delay(10);
    }
}
