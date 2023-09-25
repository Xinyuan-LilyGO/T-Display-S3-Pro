/**
 * @file      USB_HID_Example.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-09-22
 * @note
 * * 1. After the sketch is uploaded, the ESP32S3 port will disappear and enter the host mode.
 * *    At this time, you need to connect the mouse through USBC 2 USBA,
 * *    and then press the BOOT button to turn on the OTG power supply state.
 * *    When the mouse is detected, the screen mouse will become operational.
 * * 2. When the program needs to be re-burned, T-Display-Pro must be manually
 * *    entered into the download mode so that the port can be generated correctly
 * *    and the sketch can be uploaded.
 * *     ** Steps to manually enter download mode **
 * *        - Connect the board via the USB cable
 * *        - Press and hold the BOOT button , While still pressing the BOOT button, press RST
 * *        - Release the RST
 * *        - Release the BOOT button
 * *        - Upload sketch
  */

#include <XPowersLib.h>
#include <AceButton.h>
#include "LV_Helper.h"
#include "usb_hid.h"
#include "utilities.h"

static QueueHandle_t xQueue1;
using namespace ace_button;
PowersSY6970 PMU;
AceButton button;


// The event handler for the button.
void handleEvent(AceButton *button, uint8_t eventType, uint8_t buttonState)
{
    // Print out a message for all events.
    Serial.print(F("handleEvent(): eventType: "));
    Serial.print(AceButton::eventName(eventType));
    Serial.print(F("; buttonState: "));
    Serial.println(buttonState);

    uint8_t id = button->getId();
    if (eventType != AceButton::kEventClicked) {
        return;
    }
    if (PMU.isEnableOTG()) {
        PMU.disableOTG();
        Serial.println("disableOTG");
    } else {
        PMU.enableOTG(); Serial.println("enableOTG");
        PMU.enableCharge();
    }

}

void setup()
{
    Serial.begin(115200);

    Serial.println("USB Host example");

    struct HIDMessage msg;

    xQueue1 = xQueueCreate( 10, sizeof( msg  ) );


    // initialize the corresponding AceButton
    uint8_t pin = 0;    //BOOT Pin
    pinMode(pin, INPUT_PULLUP);
    button.init(pin, HIGH);
    // Configure the ButtonConfig with the event handler, and enable all higher
    // level events.
    ButtonConfig *buttonConfig = ButtonConfig::getSystemButtonConfig();
    buttonConfig->setEventHandler(handleEvent);
    buttonConfig->setFeature(ButtonConfig::kFeatureClick);

    int maxX, maxY;

    lv_helper(xQueue1, &maxX, &maxY);

    setupUSBHID(xQueue1, maxX, maxY);


    bool hasPMU =  PMU.init(Wire, BOARD_I2C_SDA, BOARD_I2C_SCL, SY6970_SLAVE_ADDRESS);
    if (!hasPMU) {
        Serial.println("Failed to find Power management !");
    } else {
        // Set the charging target voltage, Range:3840 ~ 4608mV ,step:16 mV
        PMU.setChargeTargetVoltage(4208);

        // Set the precharge current , Range: 64mA ~ 1024mA ,step:64mA
        PMU.setPrechargeCurr(64);

        // Set the charging current , Range:0~5056mA ,step:64mA
        PMU.setChargerConstantCurr(640);

        // // To obtain voltage data, the ADC must be enabled first
        PMU.enableADCMeasure();

        PMU.enableCharge();

        PMU.disableOTG();

        PMU.enableStatLed();

        Serial.println("Find Power management");
    }


    lv_obj_t *label1 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
    lv_label_set_text(label1, "#0000ff Re-color# #ff00ff words# #ff0000 of a# label, align the lines to the center "
                      "and wrap long text automatically.");
    lv_obj_set_width(label1, 150);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, -40);

    lv_obj_t *label2 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label2, LV_LABEL_LONG_SCROLL_CIRCULAR);     /*Circular scroll*/
    lv_obj_set_width(label2, 150);
    lv_label_set_text(label2, "It is a circularly scrolling text. ");
    lv_obj_align(label2, LV_ALIGN_CENTER, 0, 20);


    lv_obj_t *label;

    lv_obj_t *btn1 = lv_btn_create(lv_scr_act());
    lv_obj_align(btn1, LV_ALIGN_BOTTOM_MID, -80, -40);

    label = lv_label_create(btn1);
    lv_label_set_text(label, "Button");
    lv_obj_center(label);

    lv_obj_t *btn2 = lv_btn_create(lv_scr_act());
    lv_obj_align(btn2, LV_ALIGN_BOTTOM_MID, 80, -40);
    lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_height(btn2, LV_SIZE_CONTENT);

    label = lv_label_create(btn2);
    lv_label_set_text(label, "Toggle");
    lv_obj_center(label);

}


void loop()
{
    button.check();
    lv_task_handler();
    delay(2);
}





















