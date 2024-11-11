/**
 * @file      LTR553ALS_Sensor.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-09-25
 *
 */
#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <SensorLTR553.hpp>
#include "utilities.h"
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

SensorLTR553 als;

void setup()
{
    Serial.begin(115200);


    // Initialize screen , The backlight Pin has been configured in TFT_eSPI(TFT_eSPI/User_Setups/Setup214_LilyGo_T_Display_S3_Pro.h).
    // By default, the backlight is turned on after initializing the TFT.
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    // Simple show als result
    tft.setSwapBytes(true);
    tft.pushColors((uint8_t *)gImage_image, tft.width() * tft.height() * 2);
    spr.setColorDepth(8);
    spr.createSprite(tft.width(), 60);
    spr.fillSprite(TFT_LIGHTGREY);
    spr.setTextDatum(CL_DATUM);
    spr.setTextColor(TFT_BLACK, TFT_LIGHTGREY);
    spr.drawString("Powered By LilyGo ST7796 222x480 IPS TFT", 60, 10, 2);
    spr.pushSprite(0, tft.height() - 60);


    pinMode(BOARD_SENSOR_IRQ, INPUT_PULLUP);

    if (!als.begin(Wire, LTR553_SLAVE_ADDRESS, BOARD_I2C_SDA, BOARD_I2C_SCL)) {
        Serial.println("Failed to find LTR553 - check your wiring!");
        spr.drawString("Powered By LilyGo ST7796 222x480 IPS TFT", 60, 10, 2);
        spr.drawString("Failed to find LTR553 - check your wiring!", 5, 30, 2);
        spr.pushSprite(0, tft.height() - 60);
        spr.fillSprite(TFT_LIGHTGREY);
        while (1) {
            delay(1000);
        }
    }

    Serial.println("Init LTR553 Sensor success!");

    // Set the ambient light high and low thresholds.
    // If the value exceeds or falls below the set value, an interrupt will be triggered.
    als.setLightSensorThreshold(10, 200);

    // Set the high and low thresholds of the proximity sensor.
    // If the value exceeds or falls below the set value, an interrupt will be triggered.
    // als.setProximityThreshold(10, 30);

    // Controls the Light Sensor N number of times the measurement data is outside the range
    // defined by the upper and lower threshold limits before asserting the interrupt.
    als.setLightSensorPersists(1);

    // Controls the Proximity  N number of times the measurement data is outside the range
    // defined by the upper and lower threshold limits before asserting the interrupt.
    als.setProximityPersists(1);

    /*
    *  ALS_IRQ_ACTIVE_LOW, // INT pin is considered active when it is a logic 0 (default)
    *  ALS_IRQ_ACTIVE_HIGH // INT pin is considered active when it is a logic 1
    * * */
    als.setIRQLevel(SensorLTR553::ALS_IRQ_ACTIVE_LOW);

    /*
    *  ALS_IRQ_ONLY_PS,    // Only PS measurement can trigger interrupt
    *  ALS_IRQ_ONLY_ALS,   // Only ALS measurement can trigger interrupt
    *  ALS_IRQ_BOTH,       // Both ALS and PS measurement can trigger interrupt
    * * * */
    als.enableIRQ(SensorLTR553::ALS_IRQ_BOTH);

    /*
    *   ALS_GAIN_1X  ,   -> 1 lux to 64k lux (default)
    *   ALS_GAIN_2X  ,   -> 0.5 lux to 32k lux
    *   ALS_GAIN_4X  ,   -> 0.25 lux to 16k lux
    *   ALS_GAIN_8X  ,   -> 0.125 lux to 8k lux
    *   ALS_GAIN_48X ,   -> 0.02 lux to 1.3k lux
    *   ALS_GAIN_96X ,   -> 0.01 lux to 600 lux
    * */
    als.setLightSensorGain(SensorLTR553::ALS_GAIN_1X);

    /*
    *   PS_LED_PLUSE_30KHZ,
    *   PS_LED_PLUSE_40KHZ,
    *   PS_LED_PLUSE_50KHZ,
    *   PS_LED_PLUSE_60KHZ,
    *   PS_LED_PLUSE_70KHZ,
    *   PS_LED_PLUSE_80KHZ,
    *   PS_LED_PLUSE_90KHZ,
    *   PS_LED_PLUSE_100KHZ,
    * * * * * * * * * * */
    als.setPsLedPulsePeriod(SensorLTR553::PS_LED_PLUSE_100KHZ);


    /*
    *   PS_LED_DUTY_25,
    *   PS_LED_DUTY_50,
    *   PS_LED_DUTY_75,
    *   PS_LED_DUTY_100,
    * * * */
    als.setPsLedDutyCycle(SensorLTR553::PS_LED_DUTY_100);


    /*
    *   PS_LED_CUR_5MA,
    *   PS_LED_CUR_10MA,
    *   PS_LED_CUR_20MA,
    *   PS_LED_CUR_50MA,
    *   PS_LED_CUR_100MA,
    * * * * * * * */
    als.setPsLedCurrent(SensorLTR553::PS_LED_CUR_100MA);

    /*
    *   PS_MEAS_RATE_50MS,
    *   PS_MEAS_RATE_70MS,
    *   PS_MEAS_RATE_100MS,
    *   PS_MEAS_RATE_200MS,
    *   PS_MEAS_RATE_500MS,
    *   PS_MEAS_RATE_1000MS,
    *   PS_MEAS_RATE_2000MS,
    *   PS_MEAS_RATE_10MS
    * * * * * * * */
    als.setProximityRate(SensorLTR553::PS_MEAS_RATE_50MS);

    // Number of pulses
    als.setPsLedPulses(1);

    // Enable ambient light sensor
    als.enableLightSensor();

    // Enable proximity sensor
    als.enableProximity();

    // Enable proximity sensor saturation indication
    als.enablePsIndicator();
}


void loop()
{
    bool saturated = false;

    uint16_t ch0 = als.getLightSensor(0);
    uint16_t ch1 = als.getLightSensor(1);
    uint16_t ps = als.getProximity(&saturated);


    Serial.print(" IRQ:"); Serial.print (digitalRead(BOARD_SENSOR_IRQ));
    Serial.print(" ALS: CH1:"); Serial.print(ch1);
    Serial.print(" -  CH0:"); Serial.print(ch0);
    Serial.print(" -  PS:"); Serial.print(ps);
    Serial.print(" -  "); Serial.println(saturated ? "PS saturated" : "PS not saturated");



    spr.drawString("Powered By LilyGo ST7796 222x480 IPS TFT", 60, 10, 2);
    char buf[256];
    snprintf(buf, 256, "[CH0]:%u [CH1]:%u [PS]:%u", ch0, ch1, ps);
    spr.drawString(buf, 5, 30, 2);
    spr.pushSprite(0, tft.height() - 60);
    spr.fillSprite(TFT_LIGHTGREY);
    delay(500);
}



