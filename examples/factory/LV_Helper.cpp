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
TouchDrvCSTXXX touch;
static home_button_callback_t homeCb = NULL;


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
        data->point.x = x[0];
        data->point.y = y[0];
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
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
    Serial.print ("setBrightness : "); Serial.println(level);
#endif
}

void lv_touch_homekey_set_cb(home_button_callback_t  cb)
{
    homeCb = cb;
}

void touchHomeKeyCallback(void *user_data)
{
    Serial.println("Home key pressed!");
    static uint32_t checkMs = 0;
    if (millis() > checkMs) {
        if (homeCb) {
            homeCb(NULL);
        }
    }
    checkMs = millis() + 200;
}




void lv_helper(uint8_t r)
{
    static lv_color_t *frameBuffer;
    static lv_disp_drv_t disp_drv;
    static lv_disp_draw_buf_t draw_buf;




    // Init Display
    tft.init();
    tft.fillScreen(TFT_BLACK);
    tft.setRotation(r);

    touch.setPins(BOARD_TOUCH_RST, BOARD_SENSOR_IRQ);
    bool hasTouch = touch.begin(Wire, CST226SE_SLAVE_ADDRESS, BOARD_I2C_SDA, BOARD_I2C_SCL);
    if (!hasTouch) {
        Serial.println("Failed to find Capacitive Touch !");
    } else {
        Serial.println("Find Capacitive Touch");
        touch.setMaxCoordinates(TFT_HEIGHT, TFT_WIDTH);
        touch.setSwapXY(true);
        touch.setMirrorXY(false, true);
        touch.setHomeButtonCallback(touchHomeKeyCallback, NULL);
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



























