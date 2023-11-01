/**
 * @file      AdjustBacklight.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-11-01
 *
 */
#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <TFT_eSPI.h>
#include "utilities.h"

// TFT Pin check
#if BOARD_SPI_MOSI  != TFT_MOSI || \
    BOARD_SPI_SCK  != TFT_SCLK || \
    BOARD_TFT_CS    != TFT_CS   || \
    BOARD_TFT_DC    != TFT_DC   || \
    BOARD_TFT_RST   != TFT_RST || \
    TFT_WIDTH       != BOARD_TFT_WIDTH || \
    TFT_HEIGHT      != BOARD_TFT_HEIHT
#error  "Error! Please make sure <Setup214_LilyGo_T_Display_S3_Pro.h> is selected in <TFT_eSPI/User_Setup_Select.h>"
#error  "Error! Please make sure <Setup214_LilyGo_T_Display_S3_Pro.h> is selected in <TFT_eSPI/User_Setup_Select.h>"
#error  "Error! Please make sure <Setup214_LilyGo_T_Display_S3_Pro.h> is selected in <TFT_eSPI/User_Setup_Select.h>"
#error  "Error! Please make sure <Setup214_LilyGo_T_Display_S3_Pro.h> is selected in <TFT_eSPI/User_Setup_Select.h>"
#endif

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);


void setBrightness(uint8_t value)
{
#ifdef USING_DISPLAY_PRO_V1
    ledcWrite(LEDC_TFT_CH, value);
#else
    static uint8_t level = 0;
    static uint8_t steps = 16;
    if (value == 0) {
        digitalWrite(BOARD_TFT_BL, 0);
        delay(3);
        level = 0;
        return;
    }
    if (level == 0) {
        digitalWrite(BOARD_TFT_BL, 1);
        level = steps;
        delayMicroseconds(30);
    }
    int from = steps - level;
    int to = steps - value;
    int num = (steps + to - from) % steps;
    for (int i = 0; i < num; i++) {
        digitalWrite(BOARD_TFT_BL, 0);
        digitalWrite(BOARD_TFT_BL, 1);
    }
    level = value;
#endif
}


void setup()
{
    Serial.begin(115200);

    // Initialize screen , The backlight Pin has been configured in TFT_eSPI(TFT_eSPI/User_Setups/Setup214_LilyGo_T_Display_S3_Pro.h).
    // By default, the backlight is turned on after initializing the TFT.
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    spr.setColorDepth(8);
    spr.createSprite(tft.width(), 60);
    spr.fillSprite(TFT_LIGHTGREY);
    spr.setTextDatum(CL_DATUM);
    spr.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    spr.pushSprite(0, tft.height() / 2 - 30);


    // Initialize backlight
#ifdef USING_DISPLAY_PRO_V1
    // v1.0 uses PWM for backlight adjustment
    ledcSetup(LEDC_TFT_CH, 1000, 8);
    ledcAttachPin(BOARD_TFT_BL, LEDC_TFT_CH);
    ledcWrite(LEDC_TFT_CH, 0);
    for (int i = 0; i <= BRIGHTNESS_MAX_LEVEL; ++i) {
        ledcWrite(LEDC_TFT_CH, i);
        delay(1);
    }
#else
    // v1.1 has 16 levels of range adjustment
    pinMode(BOARD_TFT_BL, OUTPUT);
#endif
}

void loop()
{

    for (int i = BRIGHTNESS_MAX_LEVEL; i >= 0; --i) {
        setBrightness( i);
        String buf = "Set brightness : " + String(i);
        spr.drawString(buf, 5, 30, 2);
        spr.pushSprite(0, tft.height() / 2 - 30);
        spr.fillSprite(TFT_BLACK);
        delay(10);
    }

    delay(2000);

    for (int i = 0; i <= BRIGHTNESS_MAX_LEVEL; ++i) {
        setBrightness( i);
        String buf = "Set brightness : " + String(i);
        spr.drawString(buf, 5, 30, 2);
        spr.pushSprite(0, tft.height() / 2 - 30);
        spr.fillSprite(TFT_BLACK);
        delay(10);
    }
    delay(2000);
}



