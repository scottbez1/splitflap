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

DisplayTask::DisplayTask(SplitflapTask& splitflap_task, const uint8_t task_core) : Task{"Display", 6000, 1, task_core}, splitflap_task_{splitflap_task} {}

// static const int32_t MODULE_WIDTH = 20;
// static const int32_t MODULE_HEIGHT = 26;
// static const uint8_t TEXT_SIZE = 3;

static const int32_t MODULE_WIDTH = 10;
static const int32_t MODULE_HEIGHT = 10;
static const uint8_t TEXT_SIZE = 1;

void DisplayTask::run() {
    tft_.begin();
    tft_.invertDisplay(1);
    tft_.setRotation(1);

    // spr_.setColorDepth(8);
    // spr_.createSprite(TFT_HEIGHT, TFT_WIDTH);
    // spr_.setTextFont(0);
    // spr_.setTextSize(TEXT_SIZE);
    // spr_.setTextColor(0xFFFF, TFT_BLACK);

    // spr_.fillSprite(TFT_BLACK);
    // spr_.fillRect(MODULE_WIDTH, MODULE_HEIGHT, COLUMNS * (MODULE_WIDTH + 1) + 1, ((NUM_MODULES + COLUMNS - 1) / COLUMNS) * (MODULE_HEIGHT + 1) + 1, 0x2104);

    tft_.setTextFont(0);
    tft_.setTextSize(TEXT_SIZE);
    tft_.setTextColor(0xFFFF, TFT_BLACK);

    tft_.fillScreen(TFT_BLACK);
    tft_.fillRect(MODULE_WIDTH, MODULE_HEIGHT, COLUMNS * (MODULE_WIDTH + 1) + 1, ((NUM_MODULES + COLUMNS - 1) / COLUMNS) * (MODULE_HEIGHT + 1) + 1, 0x2104);

    uint8_t module_row, module_col;
    int32_t module_x, module_y;
    SplitflapState last_state = {};
    while(1) {
        SplitflapState state = splitflap_task_.getState();
        if (state != last_state) {
            for (uint8_t i = 0; i < NUM_MODULES; i++) {
                SplitflapModuleState& s = state.modules[i];
                if (s == last_state.modules[i]) {
                    continue;
                }

                uint16_t background = 0x0000;
                uint16_t foreground = 0xFFFF;

                bool blink = (millis() / 400) % 2;

                char c;
                switch (s.state) {
                    case NORMAL:
                        c = flaps[s.flap_index];
                        if (s.moving) {
                            // use a dimmer color when moving
                            foreground = 0x6b4d;
                        }

                        // You can add special-case color handling here if desired:
                        // if (c == 'w') {
                        //     c = ' ';
                        //     background = 0xFFFF;
                        // } else if (c == 'y') {
                        //     c = ' ';
                        //     background = 0xffe0;
                        // } else if (c == 'o') {
                        //     c = ' ';
                        //     background = 0xfd00;
                        // } else if (c == 'g') {
                        //     c = ' ';
                        //     background = 0x46a0;
                        // } else if (c == 'p') {
                        //     c = ' ';
                        //     background = 0xd938;
                        // }
                        break;
                    case PANIC:
                        c = '~';
                        background = blink ? 0xD000 : 0;
                        break;
                    case STATE_DISABLED:
                        c = '*';
                        break;
                    case LOOK_FOR_HOME:
                        c = '?';
                        background = blink ? 0x6018 : 0;
                        break;
                    case SENSOR_ERROR:
                        c = ' ';
                        background = blink ? 0xD461 : 0;
                        break;
                    default:
                        c = ' ';
                        break;
                }
                getLayoutPosition(i, &module_row, &module_col);

                // Leave an empty row and column for headers, and add 1 to width/height as a separator line between modules
                module_x = (module_col + 1) * (MODULE_WIDTH + 1);
                module_y = (module_row + 1) * (MODULE_HEIGHT + 1);

                tft_.setTextColor(foreground, background);
                tft_.fillRect(module_x, module_y, MODULE_WIDTH, MODULE_HEIGHT, background);
                tft_.setCursor(module_x + 1, module_y + 2);
                tft_.printf("%c", c);
            }
            // spr_.pushSprite(0, 0);
            last_state = state;

        }


        delay(10);
    }
}
