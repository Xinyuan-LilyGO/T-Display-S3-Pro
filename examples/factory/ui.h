#pragma once

#ifndef UI_H 
#define UI_H 

#include "Arduino.h"

#define LV_DELAY(x)                                                                                                                                  \
do {                                                                                                                                               \
  uint32_t t = x;                                                                                                                                  \
  while (t--) {                                                                                                                                    \
    lv_timer_handler();                                                                                                                            \
    delay(1);                                                                                                                                      \
  }                                                                                                                                                \
} while (0);

void ui_init(void);
void return_home_cd(void);
void wifi_connect_cd(void);
void set_usbAndBattery(uint16_t usb_sats, uint16_t usb_voltage, uint16_t battery);

//void ui_boot_anim();


#endif 
