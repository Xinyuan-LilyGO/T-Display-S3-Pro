#include <Arduino.h>

#include "TFT_eSPI.h"
#include "lvgl.h"
#include "src/logo.h"

#define TOUCH_MODULES_CST_MUTUAL
#include "TouchLib.h"
#include <Wire.h>

uint8_t TOUCH_ADDRESS = 0x5A;
#define TOUCH_IICSCL  6
#define TOUCH_IICSDA  5
#define TOUCH_INT     7
#define TOUCH_RES     21
static EventGroupHandle_t touch_eg;
TouchLib touch(Wire, TOUCH_IICSDA, TOUCH_IICSCL, TOUCH_ADDRESS);

static const uint16_t screenWidth = 222;
static const uint16_t screenHeight = 480;
TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight);

extern uint8_t theme_bg_img0_map[];

static lv_color_t *buf = NULL;
static lv_disp_draw_buf_t draw_buf;

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area,
                   lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  #ifdef RM69330_DRIVER
      tft.pushImage(area->x1+6, area->y1, w, h, (uint16_t *)&color_p->full);
  #else 
      tft.pushImage(area->x1, area->y1, w, h, (uint16_t *)&color_p->full);
  #endif
  
  lv_disp_flush_ready(disp);
}

lv_obj_t* ui_label = NULL;

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    static char touchpad_but = 0;

    if (touch.read()) {

      TP_Point t = touch.getPoint(0);

      uint16_t x = t.x;
      uint16_t y = t.y;

      if((x > screenWidth) && (x <= 600))
      {
          if(!touchpad_but)
          {
              touchpad_but = 1;
          }
          return;
      }

      data->state = LV_INDEV_STATE_PR;
      data->point.x = x;
      data->point.y = y;

     char str[25] = {0};
     snprintf(str, 24, "(x = %ld, y = %ld)\n", (long int)x, (long int)y);
      lv_label_set_text(ui_label, str);
  }
  else 
  {
      if(touchpad_but)
      {
          touchpad_but = 0;
      }
      data->state = LV_INDEV_STATE_REL;
  }
}

void setup() {
    Serial.begin(115200);
    Serial.println("init sta\n");
    delay(10);

    pinMode(TOUCH_RES, OUTPUT);
    digitalWrite(TOUCH_RES, 0);delay(100);
    digitalWrite(TOUCH_RES, 1);delay(100);

    tft.begin();
    tft.setRotation(0);
    tft.setSwapBytes(true);

    tft.fillScreen(TFT_BLACK);

    pinMode(48, OUTPUT);
    digitalWrite(48, HIGH);

    touch_eg = xEventGroupCreate();
    Wire.begin(TOUCH_IICSDA, TOUCH_IICSCL);
    touch.init();

    tft.setTextColor(TFT_WHITE, TFT_BLACK);

        lv_init();

    size_t buffer_size =
        sizeof(lv_color_t) * screenWidth * screenHeight;
    buf = (lv_color_t *)ps_malloc(buffer_size);
    if (buf == NULL) {
        while (1) {
        Serial.println("buf NULL");
        delay(500);
        }
    }
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, buffer_size);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);

    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    LV_IMG_DECLARE(theme_bg_img0);
    lv_obj_t* ui_home = lv_obj_create(lv_scr_act());
    lv_obj_clear_flag(ui_home, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(ui_home, 0, 0);
    lv_obj_set_x(ui_home, 0);
    lv_obj_set_y(ui_home, 0);
    lv_obj_set_width(ui_home, 222);
    lv_obj_set_height(ui_home, 480);
    lv_obj_set_style_bg_img_src(ui_home, &theme_bg_img0, LV_PART_MAIN);

    ui_label = lv_label_create(lv_layer_top());
    lv_obj_set_width(ui_label, 150);
    lv_obj_set_height(ui_label, 30);
    lv_obj_set_x(ui_label, 60);
    lv_obj_set_y(ui_label, 240);
    lv_label_set_text(ui_label, "(x = 0, y = 0)");

    Serial.println("init end\n");
}

void loop() {
    lv_timer_handler();
    delay(2);
}
