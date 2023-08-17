#include <Arduino.h>

#include "lvgl.h"
#include "TFT_eSPI.h"
#include <Wire.h>

#include <LTR55x.h>
LTR55x ltr55x;

static const uint16_t screenWidth = 222;
static const uint16_t screenHeight = 480;
TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight);

extern uint8_t theme_bg_img0_map[];

#define LTR_553SCL  6
#define LTR_553SDA  5

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

    tft.pushImage(0, 0, 222, 480, (uint16_t *)theme_bg_img0_map);

    Wire.begin(LTR_553SDA, LTR_553SCL);
    ltr55x.begin();
}

char als_value_str[30] = {0};
char als_ps_value[30] = {0};

void loop() {

    uint32_t als_value = ltr55x.get_als_value();
    uint32_t ps_value = ltr55x.get_ps_value();

    snprintf(als_value_str, 28, "als_value = %u   \n", als_value);
    snprintf(als_ps_value, 28, "ps_value = %u     \n", ps_value);

    Serial.println(als_value_str);
    Serial.println(als_ps_value);
    tft.drawString(als_value_str, 40, 230, 2);
    tft.drawString(als_ps_value, 40, 250, 2);

    delay(100);
}
