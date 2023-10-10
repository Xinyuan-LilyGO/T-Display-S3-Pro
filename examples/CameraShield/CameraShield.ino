/**
 * @file      CameraShield.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-09-25
 * @note      Sketch Adaptation T-Display-S3-Pro Camera Shield , see README
 *            When turning on OTG power supply, please turn off the physical switch, then remove the
 *            USB power supply, and then turn on the physical switch.
 *            If OTG power supply is turned on when USB is connected, the device will not output OTG power supply.
 */
#include <Arduino.h>
#include <esp_camera.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <XPowersLib.h>
#include <AceButton.h>
#include "utilities.h"

using namespace ace_button;

#if ARDUINO_USB_CDC_ON_BOOT != 1
#warning "If you need to monitor printed data, be sure to set USB CDC On boot to ENABLE, otherwise you will not see any data in the serial monitor"
#endif

#ifndef BOARD_HAS_PSRAM
#error "Detected that PSRAM is not turned on. Please set PSRAM to OPI PSRAM in ArduinoIDE"
#endif


TFT_eSPI tft = TFT_eSPI();
PowersSY6970 PMU;
AceButton buttons[BOARD_USER_BTN_NUM];
uint8_t buttonsArray[BOARD_USER_BTN_NUM] = BOARD_USER_BUTTON;
bool hasCamera;
camera_config_t config;

static bool  led_state = 1;


// The event handler for the button.
void handleEvent(AceButton *button, uint8_t eventType, uint8_t buttonState)
{
    // Print out a message for all events.
    Serial.print(F("handleEvent(): eventType: "));
    Serial.print(AceButton::eventName(eventType));
    Serial.print(F("; buttonState: "));
    Serial.println(buttonState);

    // Get the LED pin
    uint8_t id = button->getId();

    if (eventType != AceButton::kEventClicked) {
        return;
    }
    switch (id) {
    case 0:
        break;
    case 1:
        led_state ? ledcWrite(LEDC_WHITE_CH, 0) : ledcWrite(LEDC_WHITE_CH, 20) ;
        led_state = !led_state;
        break;
    case 2:
        // The LED uses 5V power supply. If battery power is used, it is necessary to turn on the OTG power supply of the PMU to power the LED
        if (PMU.isEnableOTG()) {
            PMU.disableOTG(); Serial.println("disableOTG");

        } else {
            PMU.enableOTG(); Serial.println("enableOTG");
        }
        break;
    default:
        break;
    }
}

void buttonHandler(void *)
{
    while (1) {
        // Should be called every 4-5ms or faster, for the default debouncing time
        // of ~20ms.
        for (uint8_t i = 0; i < BOARD_USER_BTN_NUM; i++) {
            buttons[i].check();
        }
        delay(5);
    }
}

void setup()
{
    Serial.begin(115200);
    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(TFT_RED);

    bool hasPMU =  PMU.init(Wire, BOARD_I2C_SDA, BOARD_I2C_SCL, SY6970_SLAVE_ADDRESS);
    if (!hasPMU) {
        Serial.println("Failed to find Power management !");
        tft.setCursor(100, 80);
        tft.println("Failed to find Power management !");
    } else {
        // Set the charging target voltage, Range:3840 ~ 4608mV ,step:16 mV
        PMU.setChargeTargetVoltage(3856);

        // Set the precharge current , Range: 64mA ~ 1024mA ,step:64mA
        PMU.setPrechargeCurr(64);

        // Set the charging current , Range:0~5056mA ,step:64mA
        PMU.setChargerConstantCurr(320);

        // To obtain voltage data, the ADC must be enabled first
        PMU.enableADCMeasure();

        // Turn off PMU battery load
        PMU.disableBattery();

        // Turn off the PMU charging indicator light
        PMU.disableStatLed();

        Serial.println("Find Power management");
    }

    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = CAMERA_PIN_Y2;
    config.pin_d1 = CAMERA_PIN_Y3;
    config.pin_d2 = CAMERA_PIN_Y4;
    config.pin_d3 = CAMERA_PIN_Y5;
    config.pin_d4 = CAMERA_PIN_Y6;
    config.pin_d5 = CAMERA_PIN_Y7;
    config.pin_d6 = CAMERA_PIN_Y8;
    config.pin_d7 = CAMERA_PIN_Y9;
    config.pin_xclk = CAMERA_PIN_XCLK;
    config.pin_pclk = CAMERA_PIN_PCLK;
    config.pin_vsync = CAMERA_PIN_VSYNC;
    config.pin_href = CAMERA_PIN_HREF;
    config.pin_sccb_sda = CAMERA_PIN_SIOD;
    config.pin_sccb_scl = CAMERA_PIN_SIOC;
    config.pin_pwdn = CAMERA_PIN_PWDN;
    config.pin_reset = CAMERA_PIN_RESET;
    config.xclk_freq_hz = XCLK_FREQ_HZ;
    config.pixel_format = PIXFORMAT_RGB565;
    config.frame_size = FRAMESIZE_HVGA;
    config.jpeg_quality = 12;
    config.fb_count = 2;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;

    // camera init
    esp_err_t  err =  esp_camera_init(&config) ;
    if (err != ESP_OK) {
        tft.setCursor(100, 100);
        tft.print("Camera init FAIL,"); tft.print("err = "); tft.println(err);
    }

    hasCamera = err == ESP_OK;

    sensor_t *s = esp_camera_sensor_get();
    if (s) {
        Serial.print("camera id:");
        Serial.println(s->id.PID);
        camera_sensor_info_t *sinfo = esp_camera_sensor_get_info(&(s->id));
        if (sinfo) {
            Serial.print("camera model:");
            Serial.println(sinfo->name);
        }
        if (s->id.PID == GC0308_PID) {
            s->set_vflip(s, 0);
            s->set_hmirror(s, 0);
        }
    }


    // Initialize Camera LED
    // Adjust the LED duty cycle to save power and heat.
    // If you directly set the LED to HIGH, the heat brought by the LED will be huge,
    // and the current consumption will also be huge.
    ledcSetup(LEDC_WHITE_CH, 1000, 8);
    ledcAttachPin(CAMERA_WHITH_LED, LEDC_WHITE_CH);
    ledcWrite(LEDC_WHITE_CH, 0);
    ledcWrite(LEDC_WHITE_CH, 20);

    for (uint8_t i = 0; i < BOARD_USER_BTN_NUM; i++) {
        // Button uses the built-in pull up register.
        pinMode(buttonsArray[i], INPUT_PULLUP);
        // initialize the corresponding AceButton
        buttons[i].init(buttonsArray[i], HIGH, i);
    }

    // Configure the ButtonConfig with the event handler, and enable all higher
    // level events.
    ButtonConfig *buttonConfig = ButtonConfig::getSystemButtonConfig();
    buttonConfig->setEventHandler(handleEvent);
    buttonConfig->setFeature(ButtonConfig::kFeatureClick);


    xTaskCreate(buttonHandler, "btn", 2 * 1024, NULL, 10, NULL);

}


void loop()
{
    if (!hasCamera)return;
    camera_fb_t *frame = esp_camera_fb_get();
    if (frame) {
        tft.setAddrWindow(0, 0, frame->width, frame->height);
        tft.pushColors((uint16_t *)frame->buf, ( 480 * tft.height()), false);
        esp_camera_fb_return(frame);
    }
    delay(1);
}



















