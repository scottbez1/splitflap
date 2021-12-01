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
#include "display_task.h"

#include "../core/common.h"
#include "../core/semaphore_guard.h"

#include "display_layouts.h"

DisplayTask::DisplayTask(SplitflapTask& splitflap_task, const uint8_t task_core) : Task("Display", 6000, 1, task_core), splitflap_task_(splitflap_task), semaphore_(xSemaphoreCreateMutex()) {
    assert(semaphore_ != NULL);
    xSemaphoreGive(semaphore_);
}

DisplayTask::~DisplayTask() {
  if (semaphore_ != NULL) {
    vSemaphoreDelete(semaphore_);
  }
}


static const int32_t X_OFFSET = 10;
static const int32_t Y_OFFSET = 10;

void DisplayTask::run() {
    tft_.begin();
    tft_.invertDisplay(1);
    tft_.setRotation(1);

    tft_.setTextFont(0);
    tft_.setTextColor(0xFFFF, TFT_BLACK);

    tft_.fillScreen(TFT_BLACK);

    // Automatically scale display based on DISPLAY_COLUMNS (see display_layouts.h)
    int32_t module_width = 20;
    int32_t module_height = 26;
    uint8_t module_text_size = 3;

    uint8_t rows = ((NUM_MODULES + DISPLAY_COLUMNS - 1) / DISPLAY_COLUMNS);

    if (DISPLAY_COLUMNS > 16 || rows > 6) {
        module_width = 7;
        module_height = 10;
        module_text_size = 1;
    } else if (DISPLAY_COLUMNS > 10 || rows > 4) {
        module_width = 14;
        module_height = 18;
        module_text_size = 2;
    }

    tft_.fillRect(X_OFFSET, Y_OFFSET, DISPLAY_COLUMNS * (module_width + 1) + 1, rows * (module_height + 1) + 1, 0x2104);

    uint8_t module_row, module_col;
    int32_t module_x, module_y;
    SplitflapState last_state = {};
    String last_messages[countof(messages_)] = {};
    while(1) {
        SplitflapState state = splitflap_task_.getState();
        if (state != last_state) {
            tft_.setTextSize(module_text_size);
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

                // Add 1 to width/height as a separator line between modules
                module_x = X_OFFSET + 1 + module_col * (module_width + 1);
                module_y = Y_OFFSET + 1 + module_row * (module_height + 1);

                tft_.setTextColor(foreground, background);
                tft_.fillRect(module_x, module_y, module_width, module_height, background);
                tft_.setCursor(module_x + 1, module_y + 2);
                tft_.printf("%c", c);
            }
            last_state = state;
        }

        const int message_height = 10;
        const int message_text_size = 1;
        bool redraw_messages = false;
        {
            SemaphoreGuard lock(semaphore_);
            for (uint8_t i = 0; i < countof(messages_); i++) {
                if (messages_[i] != last_messages[i]) {
                    redraw_messages = true;
                    last_messages[i] = messages_[i];
                }
            }
        }
        if (redraw_messages) {
            tft_.setTextSize(message_text_size);
            tft_.setTextColor(TFT_WHITE, TFT_BLACK);
            tft_.fillRect(0, tft_.height() - message_height * countof(messages_), tft_.width(), message_height * countof(messages_), TFT_BLACK);
            for (uint8_t i = 0; i < countof(messages_); i++) {
                int y = tft_.height() - message_height * (countof(messages_) - i);
                tft_.drawString(last_messages[i], 2, y);
            }
        }

        delay(10);
    }
}

void DisplayTask::setMessage(uint8_t i, String message) {
    SemaphoreGuard lock(semaphore_);
    assert(i < countof(messages_));
    messages_[i] = message;
}
