/**
 * @file      LV_Helper.cpp
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-09-22
 *
 */

#include "LV_Helper.h"
#include "utilities.h"
#include <TFT_eSPI.h>
#include <TouchDrvCSTXXX.hpp>
#include "usb_hid.h"

LV_IMG_DECLARE(mouse_cursor_icon); /*Declare the image file.*/

TFT_eSPI tft = TFT_eSPI();
TouchDrvCSTXXX touch;
lv_indev_t  *mouse_indev = NULL;
static bool isBacklightOn = true;
static QueueHandle_t queue_in = NULL;


static void lv_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area,
                          lv_color_t *color_p)
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );
    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )&color_p->full, w * h, false );
    tft.endWrite();
    lv_disp_flush_ready( disp );
}

static void lv_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    static int16_t x[5], y[5];
    uint8_t touched = touch.getPoint(x, y, touch.getSupportTouchPoint());
    if (touched) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = y[0];
        data->point.y = tft.height() - x[0];
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

static void mouse_read(lv_indev_drv_t *indev, lv_indev_data_t *data)
{
    static  int16_t last_x;
    static int16_t last_y;
    static struct HIDMessage  msg;

    if (queue_in) {
        if (xQueueReceive(queue_in, &msg, pdTICKS_TO_MS(20)) == pdPASS) {
            if (msg.id == 'm') {
                Serial.printf("X:%d Y:%d L:%d R:%d\n", msg.x, msg.y, msg.left, msg.right);
                last_x = msg.x;
                last_y = msg.y;
                data->state = (msg.left || msg.right) ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
            }
        }
    }
    data->point.x = last_x;
    data->point.y = last_y;
}


void setBrightness(uint8_t value)
{
#ifdef USING_DISPLAY_PRO_V1
    ledcWrite(LEDC_TFT_CH, value);
#else
    static uint8_t level = 0;
    static uint8_t steps = 16;
    if (value == 0) {
        digitalWrite(BOARD_TFT_BL, 0);
        delay(3);
        level = 0;
        return;
    }
    if (level == 0) {
        digitalWrite(BOARD_TFT_BL, 1);
        level = steps;
        delayMicroseconds(30);
    }
    int from = steps - level;
    int to = steps - value;
    int num = (steps + to - from) % steps;
    for (int i = 0; i < num; i++) {
        digitalWrite(BOARD_TFT_BL, 0);
        digitalWrite(BOARD_TFT_BL, 1);
    }
    level = value;
#endif
}


void lv_helper(QueueHandle_t queue_i, int *x_max_o, int *y_max_o)
{
    static lv_color_t *frameBuffer;
    static lv_disp_drv_t disp_drv;
    static lv_disp_draw_buf_t draw_buf;


    queue_in = queue_i;


    // Init Display
    tft.init();
    tft.fillScreen(TFT_BLACK);
    tft.setRotation(1);

    touch.setPins(BOARD_TOUCH_RST, BOARD_TOUCH_IRQ);
    bool hasTouch = touch.init(Wire, BOARD_I2C_SDA, BOARD_I2C_SCL, CST226SE_SLAVE_ADDRESS);
    if (!hasTouch) {
        Serial.println("Failed to find Capacitive Touch !");
    } else {
        Serial.println("Find Capacitive Touch");
        touch.setHomeButtonCallback([](void *user_data) {
            Serial.println("Home key pressed!");
            static uint32_t checkMs = 0;
            if (millis() > checkMs) {
                if (isBacklightOn) {
                    for (int i = BRIGHTNESS_MAX_LEVEL; i >= 0; --i) {
                        setBrightness( i);
                        delay(1);
                    }
                    Serial.println("turn off!");
                    isBacklightOn = false;
                } else {
                    isBacklightOn = true;
                    for (int i = 0; i <= BRIGHTNESS_MAX_LEVEL; ++i) {
                        setBrightness( i);
                        delay(1);
                    }
                    Serial.println("turn on!");
                }
            }
            checkMs = millis() + 200;
        }, NULL);
    }

    lv_init();

    size_t buffer_size = sizeof(lv_color_t) * tft.width() * tft.height();
    frameBuffer  = (lv_color_t *)ps_malloc(buffer_size);
    assert(frameBuffer);

    lv_disp_drv_init(&disp_drv);
    lv_disp_draw_buf_init(&draw_buf, frameBuffer, NULL, buffer_size);

    disp_drv.hor_res = tft.width();
    disp_drv.ver_res = tft.height();
    disp_drv.flush_cb = lv_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lv_touchpad_read;
    lv_indev_drv_register(&indev_drv);


    /*Register a mouse input device*/
    static lv_indev_drv_t indev_mouse;
    lv_indev_drv_init( &indev_mouse );
    indev_mouse.type = LV_INDEV_TYPE_POINTER;
    indev_mouse.read_cb = mouse_read;
    mouse_indev = lv_indev_drv_register( &indev_mouse );


    if (queue_in) {
        lv_obj_t *cursor_obj;
        cursor_obj = lv_img_create(lv_scr_act());
        lv_img_set_src(cursor_obj, &mouse_cursor_icon);
        lv_indev_set_cursor(mouse_indev, cursor_obj);

        *x_max_o = lv_disp_get_hor_res(NULL) - mouse_cursor_icon.header.w;
        *y_max_o = lv_disp_get_ver_res(NULL) - mouse_cursor_icon.header.h;

    }

#ifdef USING_DISPLAY_PRO_V1
    ledcSetup(LEDC_TFT_CH, 1000, 8);
    ledcAttachPin(BOARD_TFT_BL, LEDC_TFT_CH);
    ledcWrite(LEDC_TFT_CH, 0);
    for (int i = 0; i <= BRIGHTNESS_MAX_LEVEL; ++i) {
        ledcWrite(LEDC_TFT_CH, i);
        delay(1);
    }
#else
    pinMode(BOARD_TFT_BL, OUTPUT);
#endif

}



























