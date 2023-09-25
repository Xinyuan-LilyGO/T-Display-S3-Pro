#include <Arduino.h>
#include <TFT_eSPI.h>
#include "image.h"

TFT_eSPI tft = TFT_eSPI();

void setup()
{
    tft.begin();
    tft.setRotation(1);
    tft.setSwapBytes(true);
}

void loop()
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("Test", 0, 0, 4);
    tft.drawString("1234567890", 0, 40, 2);
    tft.drawString("1234567890", 0, 80, 4);
    delay(5000);

    tft.pushImage(0, 0, tft.width(), tft.height(), (uint16_t *)gImage_image);
    delay(5000);
}
