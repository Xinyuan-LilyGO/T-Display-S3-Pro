#pragma once

#ifndef UI_H
#define UI_H

#include "Arduino.h"
#include <esp_camera.h>
#include <TFT_eSPI.h>
#include "ESP32_OV5640_AF.h"
#include "esp32-hal-log.h"
#include "sd_card.h"

#define TARGET_FOLDER "/"
#define UI_CAMERA_CANVAS 1
#define UI_PROMPT_TIME  1000

#define LV_DELAY(x)                                                                                                                                  \
do {                                                                                                                                               \
  uint32_t t = x;                                                                                                                                  \
  while (t--) {                                                                                                                                    \
    lv_timer_handler();                                                                                                                            \
    delay(1);                                                                                                                                      \
  }                                                                                                                                                \
} while (0);

void ui_init(void);
void return_home_cd(void *);
void wifi_connect_cd(void);
void set_usbAndBattery(uint16_t usb_sats, uint16_t usb_voltage, uint16_t battery);


extern void eeprom_write(int addr, uint8_t val);
//void ui_boot_anim();


#endif
