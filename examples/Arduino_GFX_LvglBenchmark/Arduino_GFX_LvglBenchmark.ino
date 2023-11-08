/*******************************************************************************
 * LVGL Benchmark
 * This is a benchmark demo for LVGL - Light and Versatile Graphics Library
 * import from: https://github.com/lvgl/lv_demos.git
 *
 * Dependent libraries:
 * LVGL: https://github.com/lvgl/lvgl.git
 *
 * LVGL Configuration file:
 * Copy your_arduino_path/libraries/lvgl/lv_conf_template.h
 * to your_arduino_path/libraries/lv_conf.h
 *
 * In lv_conf.h around line 15, enable config file:
 * #if 1 // Set it to "1" to enable content
 *
 * Then find and set:
 * #define LV_COLOR_DEPTH     16
 * #define LV_TICK_CUSTOM     1
 *
 * For SPI/parallel 8 display set color swap can be faster, parallel 16/RGB screen don't swap!
 * #define LV_COLOR_16_SWAP   1 // for SPI and parallel 8
 * #define LV_COLOR_16_SWAP   0 // for parallel 16 and RGB
 *
 * Enable LVGL Demo Benchmark:
 * #define LV_USE_DEMO_BENCHMARK 1
 *
 * Enables support for compressed fonts:
 * #define LV_USE_FONT_COMPRESSED 1
 *
 * Customize font size:
 * #define LV_FONT_MONTSERRAT_12 1
 * #define LV_FONT_DEFAULT &lv_font_montserrat_12
 ******************************************************************************/
#include "lv_demo_benchmark.h"
#include <Arduino_GFX_Library.h>
#include "touch.h"


#define GFX_BL BOARD_TFT_BL // default backlight pin, you may replace DF_GFX_BL to actual backlight pin

/* More data bus class: https://github.com/moononournation/Arduino_GFX/wiki/Data-Bus-Class */
Arduino_DataBus *bus = new Arduino_ESP32SPI(BOARD_TFT_DC, BOARD_TFT_CS, BOARD_SPI_SCK, BOARD_SPI_MOSI, BOARD_SPI_MISO);

/* More display class: https://github.com/moononournation/Arduino_GFX/wiki/Display-Class */
Arduino_GFX *gfx = new Arduino_ST7796(bus,
                                      BOARD_TFT_RST,
                                      0 /* rotation */,
                                      true /* IPS */,
                                      222,
                                      480, 49, 0);

/*******************************************************************************
 * Please config the touch panel in touch.h
 ******************************************************************************/

/* Change to your screen resolution */
static uint32_t screenWidth;
static uint32_t screenHeight;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;
static unsigned long last_ms;

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
    gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

    lv_disp_flush_ready(disp);
}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    if (touch_has_signal()) {
        if (touch_touched()) {
            data->state = LV_INDEV_STATE_PR;

            /*Set the coordinates*/
            data->point.x = touch_last_x;
            data->point.y = touch_last_y;
        } else if (touch_released()) {
            data->state = LV_INDEV_STATE_REL;
        }
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

void setup()
{
    Serial.begin(115200);
    // Serial.setDebugOutput(true);
    // while(!Serial);
    Serial.println("Arduino_GFX LVGL Benchmark example");

#ifdef GFX_EXTRA_PRE_INIT
    GFX_EXTRA_PRE_INIT();
#endif

    // Init Display
    if (!gfx->begin()) {
        Serial.println("gfx->begin() failed!");
    }
    gfx->fillScreen(BLACK);

#ifdef GFX_BL
    pinMode(GFX_BL, OUTPUT);
    digitalWrite(GFX_BL, HIGH);
#endif

    // Init touch device
    touch_init(gfx->width(), gfx->height(), gfx->getRotation());

    lv_init();

    screenWidth = gfx->width();
    screenHeight = gfx->height();
#ifdef ESP32
    disp_draw_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * screenWidth * 40, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
#else
    disp_draw_buf = (lv_color_t *)malloc(sizeof(lv_color_t) * screenWidth * 40);
#endif
    if (!disp_draw_buf) {
        Serial.println("LVGL disp_draw_buf allocate failed!");
    } else {
        lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, screenWidth * 40);

        /* Initialize the display */
        lv_disp_drv_init(&disp_drv);
        /* Change the following line to your display resolution */
        disp_drv.hor_res = screenWidth;
        disp_drv.ver_res = screenHeight;
        disp_drv.flush_cb = my_disp_flush;
        disp_drv.draw_buf = &draw_buf;
        lv_disp_drv_register(&disp_drv);

        /* Initialize the (dummy) input device driver */
        static lv_indev_drv_t indev_drv;
        lv_indev_drv_init(&indev_drv);
        indev_drv.type = LV_INDEV_TYPE_POINTER;
        indev_drv.read_cb = my_touchpad_read;
        lv_indev_drv_register(&indev_drv);

        lv_demo_benchmark();

        Serial.println("Setup done");
    }
    last_ms = millis();
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(5);
}