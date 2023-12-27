#pragma once
#include <TFT_eSPI.h>
#include <lvgl.h>
#include <Arduino.h>
#include <TouchDrvCSTXXX.hpp>
#include <Wire.h>
#include <ICM20948_WE.h>

#define SCR_ROTATION_0   0
#define SCR_ROTATION_90  1
#define SCR_ROTATION_180 2
#define SCR_ROTATION_270 3

enum{
    SCREEN_ID_0 = 0,
    SCREEN_ID_1,
    SCREEN_ID_2,
};

extern ICM20948_WE myIMU;

void ui_init(uint8_t r);
void ui_enter(void);