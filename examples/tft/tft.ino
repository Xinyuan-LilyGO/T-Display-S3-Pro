#include <Arduino.h>

#include "TFT_eSPI.h"
#include "lvgl.h"
#include "src/logo.h"

static const uint16_t screenWidth = 222;
static const uint16_t screenHeight = 480;
TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight);

extern uint8_t theme_bg_img0_map[];

void setup() {
    Serial.begin(115200);
    Serial.println("init sta\n");
    delay(10);

    tft.begin();
    tft.setRotation(0);
    tft.setSwapBytes(true);

    tft.fillScreen(TFT_BLACK);

    pinMode(48, OUTPUT);
    digitalWrite(48, HIGH);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.drawString("Test", 0, 0, 4);
    tft.drawString("1234567890", 0, 40, 2);
    tft.drawString("1234567890", 0, 80, 4);

    delay(5000);

    tft.pushImage(0, 0, 240, 320, (uint16_t *)gImage_logo);

    delay(5000);

    tft.pushImage(0, 0, 222, 480, (uint16_t *)theme_bg_img0_map);

    Serial.println("init end\n");
}

void loop() {
    delay(50);
}
