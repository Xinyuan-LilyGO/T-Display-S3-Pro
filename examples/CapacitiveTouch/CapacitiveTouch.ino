/**
 * @file      CapacitiveTouch.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-09-25
 *
 */
#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <TouchDrvCSTXXX.hpp>
#include <TFT_eSPI.h>
#include "utilities.h"
#include "image.h"

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

TouchDrvCSTXXX touch;
int16_t x[5], y[5];


void touchHomeKeyCallback(void *user_data)
{
    Serial.println("Home key pressed!");
    static uint32_t checkMs = 0;
    if (millis() > checkMs) {
        if (digitalRead(TFT_BL)) {
            digitalWrite(TFT_BL, LOW);
        } else {
            digitalWrite(TFT_BL, HIGH);
        }
    }
    checkMs = millis() + 200;
}


void setup()
{
    Serial.begin(115200);

    // Initialize screen , The backlight Pin has been configured in TFT_eSPI(TFT_eSPI/User_Setups/Setup214_LilyGo_T_Display_S3_Pro.h).
    // By default, the backlight is turned on after initializing the TFT.
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    // Initialize capacitive touch
    touch.setPins(BOARD_TOUCH_RST, BOARD_TOUCH_IRQ);
    touch.init(Wire, BOARD_I2C_SDA, BOARD_I2C_SCL, CST226SE_SLAVE_ADDRESS);

    //Set the screen to turn on or off after pressing the screen Home touch button
    touch.setHomeButtonCallback(touchHomeKeyCallback);

    tft.setSwapBytes(true);
    tft.pushColors((uint8_t *)gImage_image, tft.width() * tft.height() * 2);

    spr.setColorDepth(8);
    spr.createSprite(tft.width(), 60);
    spr.fillSprite(TFT_LIGHTGREY);
    spr.setTextDatum(CL_DATUM);
    spr.setTextColor(TFT_BLACK, TFT_LIGHTGREY);
    spr.drawString("Powered By LilyGo ST7796 222x480 IPS TFT", 60, 10, 2);
    spr.pushSprite(0, tft.height() - 60);

}

void loop()
{
    uint8_t touched = touch.getPoint(x, y, touch.getSupportTouchPoint());
    if (touched) {
        String buf = "";
        for (int i = 0; i < touched; ++i) {
            Serial.print("X[");
            Serial.print(i);
            Serial.print("]:");
            Serial.print(x[i]);
            Serial.print(" ");
            Serial.print(" Y[");
            Serial.print(i);
            Serial.print("]:");
            Serial.print(y[i]);
            Serial.print(" ");

            buf += "[" + String(i) + "]:";
            buf += String(x[i]);
            buf += " ";
            buf += " [" + String(i) + "]:";
            buf += String(y[i]);
            buf += " ";
        }
        Serial.println();
        spr.drawString("Powered By LilyGo ST7796 222x480 IPS TFT", 60, 10, 2);
        spr.drawString(buf, 5, 30, 2);
        spr.pushSprite(0, tft.height() - 60);
        spr.fillSprite(TFT_LIGHTGREY);
    }

    delay(5);
}



