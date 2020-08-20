
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
