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
#include "display.h"

Display::Display() : Task{"Display", 10000, 1, 0} {}

void Display::run() {
    tft.begin();
    tft.invertDisplay(1);
    tft.setRotation(0);

    spr.setColorDepth(16);
    spr.createSprite(TFT_WIDTH, TFT_HEIGHT);
    spr.setFreeFont(&FreeSans9pt7b);
    spr.setTextColor(0xFFFF, TFT_BLACK);
    while(1) {
        static uint32_t i = 0;
        i++;
        spr.fillSprite(TFT_BLACK);
        spr.setCursor(0, 15);
        spr.printf("test: %u\nhello\nworld\n\n%u\nbye!", i, i);
        spr.pushSprite(0, 0);
        vTaskDelay(1);
    }
}
