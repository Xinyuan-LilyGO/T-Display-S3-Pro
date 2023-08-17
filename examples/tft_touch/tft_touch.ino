#include <Arduino.h>

#include "TFT_eSPI.h"
#include "lvgl.h"
#include "src/logo.h"

#define TOUCH_MODULES_CST_MUTUAL
#include "TouchLib.h"
#include <Wire.h>

uint8_t TOUCH_ADDRESS = 0x5A;
#define TOUCH_IICSCL  6
#define TOUCH_IICSDA  5
#define TOUCH_INT     7
#define TOUCH_RES     21
static EventGroupHandle_t touch_eg;
TouchLib touch(Wire, TOUCH_IICSDA, TOUCH_IICSCL, TOUCH_ADDRESS);

static const uint16_t screenWidth = 222;
static const uint16_t screenHeight = 480;
TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight);

extern uint8_t theme_bg_img0_map[];

void setup() {
    Serial.begin(115200);
    Serial.println("init sta\n");
    delay(10);

    pinMode(TOUCH_RES, OUTPUT);
    digitalWrite(TOUCH_RES, 0);delay(100);
    digitalWrite(TOUCH_RES, 1);delay(100);

    tft.begin();
    tft.setRotation(0);
    tft.setSwapBytes(true);

    tft.fillScreen(TFT_BLACK);

    pinMode(48, OUTPUT);
    digitalWrite(48, HIGH);

    touch_eg = xEventGroupCreate();
    Wire.begin(TOUCH_IICSDA, TOUCH_IICSCL);
    touch.init();

    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.pushImage(0, 0, 222, 480, (uint16_t *)theme_bg_img0_map);

    tft.drawString("x = 0, y = 0 ", 80, 240, 2);
    Serial.println("init end\n");
}

void loop() {
    if (touch.read()) 
    {
        char str[26] = {0};

        TP_Point t = touch.getPoint(0);
        uint16_t x = t.x;
        uint16_t y = t.y;

        snprintf(str, 25, "x = %ld, y = %ld   \n", (long int)x, (long int)y);

        Serial.println(str);
        tft.drawString(str, 50, 240, 2);
    }
    delay(50);
}
