/**
 * @file      TJpg_Decoder.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2025  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2025-01-16
 *
 */

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include "utilities.h"

#if (BOARD_SPI_MISO != TFT_MISO)    \
    || (BOARD_SPI_MOSI != TFT_MOSI) \
    || (BOARD_SPI_SCK!= TFT_SCLK)  \
    || (TFT_CS!= BOARD_TFT_CS)      \
    || (TFT_DC!= BOARD_TFT_DC)      \
    || (TFT_RST!= BOARD_TFT_RST)
#error  "Error! Please make sure <Setup214_LilyGo_T_Display_S3_Pro.h> is selected in <TFT_eSPI/User_Setup_Select.h>"
#error  "Error! Please make sure <Setup214_LilyGo_T_Display_S3_Pro.h> is selected in <TFT_eSPI/User_Setup_Select.h>"
#error  "Error! Please make sure <Setup214_LilyGo_T_Display_S3_Pro.h> is selected in <TFT_eSPI/User_Setup_Select.h>"
#error  "Error! Please make sure <Setup214_LilyGo_T_Display_S3_Pro.h> is selected in <TFT_eSPI/User_Setup_Select.h>"
#endif

TFT_eSPI tft = TFT_eSPI();

// Image file name
const char *filename = "/480x222.jpg";      //There is a 480X222.jpg picture in the sample directory. Put it into the SD card

// This next function will be called during decoding of the jpeg file to
// render each block to the TFT.  If you use a different TFT library
// you will need to adapt this function to suit.
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
    // Stop further decoding as image is running off bottom of screen
    if ( y >= tft.height() ) return 0;

    // This function will clip the image block rendering automatically at the TFT boundaries
    tft.pushImage(x, y, w, h, bitmap);

    // This might work instead if you adapt the sketch to use the Adafruit_GFX library
    // tft.drawRGBBitmap(x, y, bitmap, w, h);

    // Return 1 to decode next block
    return 1;
}

void setup()
{
    Serial.begin(115200);

    // Initialize SDCard first and then initialize TFT.
    SPI.begin(BOARD_SPI_SCK, BOARD_SPI_MISO, BOARD_SPI_MOSI);

    // see if the card is present and can be initialized:
    while (!SD.begin(BOARD_SD_CS)) {
        Serial.println("Card failed, or not present");
        // delay 1000 ms retry begin sd card
        delay(1000);
    }
    Serial.println("Initialization SDCard done.");

    // Check to see if the file exists:
    if (SD.exists(filename)) {
        Serial.print(filename);
        Serial.println(" exists.");
    } else {
        Serial.print(filename);
        Serial.println(" doesn't exist.");
        while (1) {
            // don't do anything more:
            delay(1000);
        }
    }

    // Initialize TFT.
    tft.begin();
    tft.setRotation(1);
    tft.setSwapBytes(true);

    // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
    TJpgDec.setJpgScale(1);
    // The decoder must be given the exact name of the rendering function above
    TJpgDec.setCallback(tft_output);

}

void loop()
{
    // Time recorded for test purposes
    uint32_t t = millis();

    // Get the width and height in pixels of the jpeg if you wish
    uint16_t w = 0, h = 0;
    TJpgDec.getSdJpgSize(&w, &h, filename);
    Serial.print("Width = "); Serial.print(w); Serial.print(", height = "); Serial.println(h);

    // Draw the image, top left at 0,0
    TJpgDec.drawSdJpg(0, 0, filename);

    // How much time did rendering take
    t = millis() - t;
    Serial.print(t); Serial.println(" ms");
    delay(2000);
}
