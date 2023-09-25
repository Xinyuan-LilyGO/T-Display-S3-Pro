/**
 * @file      usb_hid.h
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-09-22
 *
 */

#pragma once




struct HIDMessage {
    char id;        // 'm' = mouse ,'k' = keyboard
    char key;
    bool left;
    bool right;
    int x;
    int y;
};

void setupUSBHID(QueueHandle_t queue_i, int x_max, int y_max);


