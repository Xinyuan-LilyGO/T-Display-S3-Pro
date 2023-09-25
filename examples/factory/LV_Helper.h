/**
 * @file      LV_Helper.h
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-09-22
 *
 */

#pragma once

#include <lvgl.h>
#include <Arduino.h>
#include <TouchDrvCSTXXX.hpp>

void lv_helper(uint8_t r = 0);
void lv_touch_homekey_set_cb(home_button_callback_t  cb);







