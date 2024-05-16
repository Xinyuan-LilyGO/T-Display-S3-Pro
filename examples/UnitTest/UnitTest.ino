/**
 * @file      UnitTest.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-09-14
 * @note      T-Display-Pro Factory test example
 *
 * *       Arduino Setting
 *            Tools ->
 *                  Board:"ESP32S3 Dev Module"
 *                  USB CDC On Boot:"Enable"
 *                  USB DFU On Boot:"Disable"
 *                  Flash Size : "16MB(128Mb)"
 *                  Flash Mode"QIO 80MHz
 *                  Partition Scheme:"16M Flash(3M APP/9.9MB FATFS)"
 *                  PSRAM:"OPI PSRAM"
 *                  Upload Mode:"UART0/Hardware CDC"
 *                  USB Mode:"Hardware CDC and JTAG"
 *  Arduino IDE User need move all folders in [lib folder](./lib/)  to Arduino library folder
 */

#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <TouchDrvCSTXXX.hpp>
#include <SensorLTR553.hpp>
#include <XPowersLib.h>
#include <TFT_eSPI.h>
#include <lvgl.h>
#include "utilities.h"
#include "freertos/semphr.h"
#include <SD.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_camera.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "rootCa.h"
#include "zones.h"
#include "img_converters.h"
#include <AceButton.h>
//QWIIC MPU9250 DEPENDENT
#include <MPU9250.h>
//QWIIC MPU6050 DEPENDENT
#include <I2Cdev.h>
#include <MPU6050.h>


using namespace ace_button;

#ifndef WIFI_SSID
#define WIFI_SSID           "Your WiFi SSDI"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD       "Your WiFi PASSOWRD"
#endif

#define NTP_SERVER1           "pool.ntp.org"
#define NTP_SERVER2           "time.nist.gov"
#define GMT_OFFSET_SEC        0
#define DAY_LIGHT_OFFSET_SEC  0
#define GET_TIMEZONE_API      "https://ipapi.co/timezone/"
#define DEFAULT_TIMEZONE      "CST-8"         //When the time zone cannot be obtained, the default time zone is used


#define SPI_LOCK            xSemaphoreTake(spiLock,portMAX_DELAY)
#define SPI_UNLOCK          xSemaphoreGive(spiLock)
#define UI_BG_COLOR         lv_color_make(241, 150, 23)
#define UI_FRAME_COLOR      lv_color_make(255, 255, 255)
#define UI_FONT_COLOR       lv_color_make(65, 65, 64)


LV_FONT_DECLARE(font_ali);


TFT_eSPI tft = TFT_eSPI();
TouchDrvCSTXXX touch;
PowersSY6970 PMU;
SensorLTR553 als;
WiFiClientSecure client;
HTTPClient https;
String httpBody;
AceButton buttons[BOARD_USER_BTN_NUM];
uint8_t buttonsArray[BOARD_USER_BTN_NUM] = BOARD_USER_BUTTON;
// MPU9250/MPU6050 non onboard sensors, default access from QWIIC interface
MPU9250 mpu;
MPU6050 accelgyro;

static int16_t x[5], y[5];
static bool hasPMU, hasALS, hasTouch, hasCamera, hasMPU9250, hasMPU6050, hasSD;
static bool isBacklightOn = true;
static bool manualOff = false;
static SemaphoreHandle_t spiLock;
static TaskHandle_t  vUpdateDateTimeTaskHandler = NULL;
static camera_sensor_info_t *sinfo = NULL;

#define CANVAS_WIDTH        480
#define CANVAS_HEIGHT       222
#define CANVAS_FORMART      LV_IMG_CF_TRUE_COLOR  // = RGB565

static lv_obj_t *canvas;
static lv_color_t *cbuf;
static lv_obj_t *hour_text;
static lv_obj_t *min_text;
static lv_obj_t *als_label ;
static uint8_t pageId = 0;
static bool cameraRun = false;
static bool sdRun = false;
static bool capture = false;
static lv_obj_t  *massStorageList;
static camera_config_t cameraConfig;
static bool ir_state = 1, led_state = 1;
static lv_obj_t *imageView = NULL;


void initButton();
void lv_helper();
void clockUI(lv_obj_t *parent);
void initBackLight();
bool initPMU();
bool initCapacitiveTouch();
bool initSensor();
void setBrightness(uint8_t bri);
bool initSD();
bool initCamera();
void initWiFi();
void initGUI();
void massStorageUI(lv_obj_t *parent);
void datetimeSyncTask(void *ptr);
void cameraUi(lv_obj_t *parent);
void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
void handleEvent(AceButton *button, uint8_t eventType, uint8_t buttonState);
void buttonHandler(void *);
void massStorageEventHandler(lv_event_t *e);
void saveJPEG(camera_fb_t *fb);

void serialToScreen(lv_obj_t *parent, String string,  bool result)
{
    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_set_scroll_dir(cont, LV_DIR_NONE);
    lv_obj_set_size(cont, LV_PCT(100), lv_font_get_line_height(&lv_font_montserrat_28) + 2 );

    lv_obj_t *label1 = lv_label_create(cont);
    lv_label_set_recolor(label1, true);
    lv_label_set_text(label1, string.c_str());
    lv_obj_align(label1, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t *label = lv_label_create(cont);
    lv_label_set_recolor(label, true);
    lv_label_set_text(label, result ? "#FFFFFF [# #00ff00 PASS# #FFFFFF ]#" : "#FFFFFF [# #ff0000  FAIL# #FFFFFF ]#");
    lv_obj_align(label, LV_ALIGN_RIGHT_MID, 0, 0);

    lv_obj_scroll_to_y(parent, lv_disp_get_ver_res(NULL), LV_ANIM_ON);

    int i = 200;
    while (i--) {
        lv_task_handler();
        delay(1);
    }
}


void setup()
{
    Serial.begin(115200);

    spiLock = xSemaphoreCreateMutex();
    assert(spiLock);
    xSemaphoreGive(spiLock);

    initButton();

    initCamera();

    initCapacitiveTouch();

    initPMU();

    initSensor();

    initSD();

    lv_helper();

    initBackLight();

    lv_obj_t *cont = lv_obj_create(lv_scr_act());
    lv_obj_set_style_border_opa(cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_color(cont, lv_color_black(), 0);
    lv_obj_set_style_radius(cont, 0, 0);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_center(cont);

    serialToScreen(cont, "Power management", hasPMU);
    serialToScreen(cont, "Capacitive Touch", hasTouch);
    serialToScreen(cont, "Proximity & Light sensor", hasALS);
    if (SD.cardType() != CARD_NONE) {
        serialToScreen(cont, "Mass storage #FFFFFF [# #00ff00  "
                       + String(SD.cardSize() / 1024 / 1024.0 )
                       + "MB# #FFFFFF ]#", true);
    } else {
        serialToScreen(cont, "Mass storage", false);
    }

    char buf[256];
    snprintf(buf, 256, "Camera Model (%s)", sinfo ? sinfo->name : "Shield");
    serialToScreen(cont, buf, hasCamera);

    // MPU9250/MPU6050 non onboard sensors, default access from QWIIC interface
    hasMPU9250 = mpu.setup(0x68);
    if (!hasMPU9250) {
        accelgyro.initialize();
        hasMPU6050 = accelgyro.testConnection() ;
    }

    if (hasMPU6050) {
        serialToScreen(cont, "IMU MPU6050 (Non onboard,QWIIC or Shield)", hasMPU6050);
        accelgyro.setTempSensorEnabled(true);
    } else if (hasMPU9250) {
        serialToScreen(cont, "IMU MPU9250 (Non onboard,QWIIC or Shield)", hasMPU9250);
    } else {
        serialToScreen(cont, "IMU (Non onboard,QWIIC or Shield)", false);
    }


    initWiFi();

    uint32_t endTime = millis() + 5000;
    while (millis() < endTime) {
        lv_task_handler();
        delay(1);
    }

    lv_obj_del(cont);

    initGUI();

}

void loop()
{
    char buf[256];
    switch (pageId) {
    case 0:
        break;
    case 1: {
        if (cameraRun && hasCamera) {
            static int64_t last_frame = 0;
            last_frame = micros();
            camera_fb_t *frame = esp_camera_fb_get();
            if (frame) {
                size_t frame_len = CANVAS_WIDTH * CANVAS_HEIGHT * sizeof(lv_color_t);
                /*
                * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                * GC0308 causes panic within this sketch.
                * GC0308 Please use CameraShield example.
                * The reason is suspected to be caused internally by esp32_camera. It is not a device problem.
                * Guru Meditation Error: Core  0 panic'ed (Unhandled debug exception).
                * Debug exception reason: Stack canary watchpoint triggered (cam_task)
                *
                * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                * * * */
                if (frame->len < frame_len) {
                    Serial.println("error : frame->len < frame_len");
                    esp_camera_fb_return(frame);
                    break;
                }
                memcpy(&cbuf->full, frame->buf, frame_len);
                lv_canvas_set_buffer(canvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR );
                if (capture) {
                    saveJPEG(frame);
                }
                int64_t fr_end = micros();
                int64_t frame_time = fr_end - last_frame;
                frame_time /= 1000;
                // Serial.printf("Frame Ues: %ums (%.1ffps)\n",  (uint32_t)frame_time, 1000.0 / (uint32_t)frame_time );
                lv_draw_label_dsc_t label_dsc;
                lv_draw_label_dsc_init(&label_dsc);
                snprintf(buf, 256, "Frame Ues:%ums (%.1ffps)", (uint32_t)frame_time, 1000.0 / (uint32_t)frame_time);
                label_dsc.color = lv_palette_main(LV_PALETTE_GREEN);
                lv_canvas_draw_text(canvas, 30, 20, 300, &label_dsc, buf);
                esp_camera_fb_return(frame);
            }
        }
    }
    break;
    case 2:
        if (sdRun) {
            sdRun = false;
            listDir(SD, "/", 0);
        }
        break;
    default:
        break;
    }
    lv_task_handler();
    delay(1);
}

void saveJPEG(camera_fb_t *fb)
{
    uint8_t *buffer = NULL;
    size_t size;
    if (!frame2jpg(fb, 12, &buffer, &size)) {
        Serial.println("frame2jpg fail!");
        return ;
    }
    assert(buffer);

    capture = false;
    SPI_LOCK;

    struct tm timeinfo;
    time_t now;
    time(&now);
    localtime_r(&now, &timeinfo);
    char filename[128];
    snprintf(filename, 128, "/Cam-%02d%02d-%02d%02d%02d.jpg", timeinfo.tm_mon, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    File f = SD.open(filename, FILE_WRITE, true);
    if (!f) {
        Serial.println("Open Failed!");
        SPI_UNLOCK;
        free(buffer);
        return;
    }
    f.write(buffer, size);
    f.close();
    free(buffer);
    Serial.println("Save !");
    SPI_UNLOCK;
}


void updateDateTimeTask(lv_timer_t *args)
{
    static bool val;
    lv_obj_t *obj = (lv_obj_t *)args->user_data;
    val ?  lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN) : lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    val ^= 1;

    struct tm timeinfo;
    time_t now;
    time(&now);
    localtime_r(&now, &timeinfo);
    lv_label_set_text_fmt(hour_text, "%02d", timeinfo.tm_hour);
    lv_label_set_text_fmt(min_text, "%02d", timeinfo.tm_min);

    bool saturated = false;

    static  uint8_t cnt = 0;

    // 1000ms
    if (cnt++ > 2 && hasALS) {

        uint16_t ch0 = als.getLightSensor(0);
        uint16_t ch1 = als.getLightSensor(1);
        uint16_t ps = als.getProximity(&saturated);

        lv_label_set_text_fmt(als_label, "ALS: [CH0]:%02u [CH1]:%02u [PS]:%02u", ch0, ch1, ps);
        lv_obj_align(als_label, LV_ALIGN_BOTTOM_MID, 0, 8);

    }
}

void tileview_change_cb(lv_event_t *e)
{
    static uint8_t lastId = 0;
    static uint16_t lastPageID = 0;
    lv_obj_t *tileview = lv_event_get_target(e);

    pageId = lv_obj_get_index(lv_tileview_get_tile_act(tileview));
    lv_event_code_t c = lv_event_get_code(e);
    Serial.print("Code : ");
    Serial.print(c);
    uint32_t count =  lv_obj_get_child_cnt(tileview);
    Serial.print(" Count:");
    Serial.print(count);
    Serial.print(" pageId:");
    Serial.println(pageId);
    if (pageId == 2 && lastId != 2) {
        sdRun = true;
    }
    lastId = pageId;
}

void initGUI()
{
    lv_obj_t *tileview = lv_tileview_create(lv_scr_act());
    lv_obj_align(tileview, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_set_size(tileview, LV_PCT(100), LV_PCT(100));
    lv_obj_remove_style(tileview, 0, LV_PART_SCROLLBAR);
    lv_obj_add_event_cb(tileview, tileview_change_cb, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *tv1 = lv_tileview_add_tile(tileview, 0, 0, LV_DIR_VER);
    lv_obj_t *tv2 = lv_tileview_add_tile(tileview, 0, 1, LV_DIR_VER);
    lv_obj_t *tv3 = lv_tileview_add_tile(tileview, 0, 2, LV_DIR_VER);
    lv_obj_t *tv4 = lv_tileview_add_tile(tileview, 0, 3, LV_DIR_VER);

    clockUI(tv1);

    cameraUi(tv2);

    massStorageUI(tv3);

    devicesUI(tv4);

    // MPU9250/MPU6050 non onboard sensors, default access from QWIIC interface
    if (hasMPU6050 || hasMPU9250) {

        lv_obj_t *tv5 = lv_tileview_add_tile(tileview, 0, 4, LV_DIR_VER);

        lv_obj_t *cont = lv_obj_create(tv5);

        lv_obj_set_style_radius(cont, 0, 0);
        lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
        lv_obj_center(cont);

        lv_obj_t *label1 = lv_label_create(cont);
        lv_label_set_recolor(label1, true);
        lv_obj_set_style_text_font(label1, &lv_font_montserrat_18, 0);
        lv_label_set_text(label1, "IMU");
        lv_obj_center(label1);

        lv_timer_create([](lv_timer_t *t) {
            lv_obj_t *label = (lv_obj_t *)t->user_data;

            if (hasMPU9250) {
                if (mpu.update()) {
                    Serial.print("Yaw, Pitch, Roll: ");
                    Serial.print(mpu.getYaw(), 2);
                    Serial.print(", ");
                    Serial.print(mpu.getPitch(), 2);
                    Serial.print(", ");
                    Serial.println(mpu.getRoll(), 2);
                    lv_label_set_text_fmt(label, "Yaw:%.2f\nPitch:%.2f\nRoll:%.2f", mpu.getYaw(), mpu.getPitch(), mpu.getRoll());
                }
            } else {
                int16_t ax, ay, az;
                int16_t gx, gy, gz;
                accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
                int16_t rawTemp = accelgyro.getTemperature();
                float temperature = (rawTemp / 340.0) + 36.53;
                Serial.print("a/g:\t");
                Serial.print(ax); Serial.print("\t");
                Serial.print(ay); Serial.print("\t");
                Serial.print(az); Serial.print("\t");
                Serial.print(gx); Serial.print("\t");
                Serial.print(gy); Serial.print("\t");
                Serial.println(gz);
                lv_label_set_text_fmt(label, "AX:%d AY:%d AZ:%d\nGX:%d GY:%d GZ:%d\nTemperature:%.2f",
                                      ax, ay, az, gx, gy, gz, temperature);
            }
            lv_obj_center(label);

        }, 100, label1);
    }
}

void lv_device_switch_event_handler(lv_event_t *e)
{
    if (!hasPMU)return;
    lv_event_code_t event = lv_event_get_code(e);
    if (event != LV_EVENT_VALUE_CHANGED )return;

    lv_obj_t *sw = lv_event_get_target(e);
    lv_state_t  state = lv_obj_get_state(sw);

    Serial.print("STATE:");
    Serial.println(state, HEX);
    switch (*(uint8_t *)lv_event_get_user_data(e)) {
    // "OTG Power"
    case 0:
        if (state == LV_STATE_FOCUSED) {
            Serial.println("OTG DISABLE");
            PMU.disableOTG();
        } else {
            if (!PMU.enableOTG()) {
                Serial.println("Enable OTG Failed!");
                lv_obj_clear_state(sw, LV_STATE_CHECKED);
            } else {
                Serial.println("OTG ENABLE");
            }
        }
        break;
    // "State LED Enable"
    case 1:
        if (state == LV_STATE_FOCUSED) {
            Serial.println("LED DISABLE");
            PMU.disableStatLed();
        } else {
            PMU.enableStatLed();
            Serial.println("LED ENABLE");
        }
        break;
    default:
        break;
    }
}

void devicesUI(lv_obj_t *parent)
{
    /*Init the pressed style*/
    static lv_style_t style_pr;
    lv_style_init(&style_pr);

    /*Add a large outline when pressed*/
    lv_style_set_outline_width(&style_pr, 30);
    lv_style_set_outline_opa(&style_pr, LV_OPA_TRANSP);


    lv_style_set_bg_color(&style_pr, lv_palette_darken(LV_PALETTE_ORANGE, 2));
    lv_style_set_bg_grad_color(&style_pr, lv_palette_darken(LV_PALETTE_ORANGE, 4));

    /*Add a transition to the outline*/
    static lv_style_transition_dsc_t trans;
    static lv_style_prop_t props[] = {LV_STYLE_OUTLINE_WIDTH, LV_STYLE_OUTLINE_OPA, LV_STYLE_PROP_INV};
    lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, 300, 0, NULL);

    lv_style_set_transition(&style_pr, &trans);


    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_set_style_bg_color(cont, UI_BG_COLOR, 0);
    lv_obj_set_style_radius(cont, 0, 0);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_center(cont);


    lv_obj_t *sw;
    lv_obj_t *label;

    const char *swName[] = {"OTG Power", "State LED Enable"};
    static uint8_t index[2] = {0, 1};
    for (int i = 0; i < sizeof(swName) / sizeof(swName[0]); ++i) {
        lv_obj_t *obj = lv_obj_create(cont);
        lv_obj_set_scroll_dir(obj, LV_DIR_NONE);
        lv_obj_set_size(obj, LV_PCT(100), LV_PCT(30));

        sw = lv_switch_create(obj);
        lv_obj_set_size(sw, 80, 40);
        lv_obj_align(sw, LV_ALIGN_RIGHT_MID, -20, 0);

        lv_obj_add_style(sw, &style_pr, LV_STATE_CHECKED);
        lv_obj_add_event_cb(sw, lv_device_switch_event_handler, LV_EVENT_VALUE_CHANGED, &index[i]);

        label = lv_label_create(obj);
        lv_label_set_text(label, swName[i]);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0);
        lv_obj_align(label, LV_ALIGN_LEFT_MID, 10, 0);
    }

    lv_obj_t *obj = lv_obj_create(cont);
    lv_obj_set_scroll_dir(obj, LV_DIR_NONE);
    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(30));

    label = lv_label_create(obj);
    lv_label_set_text(label, "Battery Voltage:");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 10, 0);


    lv_obj_t *label1 = lv_label_create(obj);
    lv_label_set_text(label1, "00");
    lv_obj_set_style_text_font(label1, &lv_font_montserrat_24, 0);
    lv_obj_align(label1, LV_ALIGN_LEFT_MID, 10, 0);

    lv_timer_create([](lv_timer_t *t) {
        lv_obj_t *label1 = (lv_obj_t *)t->user_data;
        lv_label_set_text_fmt(label1, "%lu mV", PMU.getBattVoltage());
        lv_obj_align(label1, LV_ALIGN_RIGHT_MID, -20, 0);
    }, 1000, label1);
}

void clockUI(lv_obj_t *parent)
{
    lv_obj_t *main_cout = lv_obj_create(parent);
    lv_obj_set_size(main_cout, LV_PCT(100), LV_PCT(100));
    lv_obj_clear_flag(main_cout, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(main_cout, 0, 0);
    lv_obj_set_style_bg_color(main_cout, UI_BG_COLOR, 0);

    lv_obj_t *hour_cout = lv_obj_create(main_cout);
    lv_obj_set_size(hour_cout, LV_PCT(40), LV_PCT(90));
    lv_obj_align(hour_cout, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_set_style_bg_color(hour_cout, UI_FRAME_COLOR, 0);
    lv_obj_clear_flag(hour_cout, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *min_cout = lv_obj_create(main_cout);
    lv_obj_set_size(min_cout, LV_PCT(40), LV_PCT(90));
    lv_obj_align(min_cout, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_set_style_bg_color(min_cout, UI_FRAME_COLOR, 0);
    lv_obj_clear_flag(min_cout, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *seg_text = lv_label_create(main_cout);
    lv_obj_align(seg_text, LV_ALIGN_CENTER, 0, -10);
    lv_obj_set_style_text_font(seg_text, &font_ali, 0);
    lv_label_set_text(seg_text, ":");
    lv_obj_set_style_text_color(seg_text, UI_FONT_COLOR, 0);

    hour_text = lv_label_create(hour_cout);
    lv_obj_center(hour_text);
    lv_obj_set_style_text_font(hour_text, &font_ali, 0);
    lv_label_set_text(hour_text, "12");
    lv_obj_set_style_text_color(hour_text, UI_FONT_COLOR, 0);


    min_text = lv_label_create(min_cout);
    lv_obj_center(min_text);
    lv_obj_set_style_text_font(min_text, &font_ali, 0);
    lv_label_set_text(min_text, "34");
    lv_obj_set_style_text_color(min_text, UI_FONT_COLOR, 0);

    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 2);
    lv_style_set_line_color(&style_line, lv_color_make(133, 133, 133));
    lv_style_set_line_rounded(&style_line, true);

    static lv_point_t line_points[] = {{-405, 0}, {405, 0}};
    lv_obj_t *line;
    line = lv_line_create(main_cout);
    lv_line_set_points(line, line_points, 2);
    lv_obj_add_style(line, &style_line, 0);
    lv_obj_center(line);

    als_label = lv_label_create(main_cout);
    lv_obj_center(als_label);
    lv_obj_set_style_text_font(als_label, &lv_font_montserrat_18, 0);
    lv_label_set_text(als_label, "dectete...");
    lv_obj_set_style_text_color(als_label, UI_FONT_COLOR, 0);
    lv_obj_align(als_label, LV_ALIGN_BOTTOM_MID, 0, 8);

    lv_timer_create(updateDateTimeTask, 500, seg_text);

}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
    if (!massStorageList) {
        return;
    }
    lv_obj_clean(massStorageList);

    Serial.printf("Listing directory: %s\n", dirname);

    SPI_LOCK;
    File root = fs.open(dirname);
    if (!root) {
        Serial.println("Failed to open directory"); SPI_UNLOCK;
        return;
    }
    if (!root.isDirectory()) {
        Serial.println("Not a directory"); SPI_UNLOCK;
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("  DIR : ");
            lv_obj_t *btn = lv_list_add_btn(massStorageList, LV_SYMBOL_DIRECTORY, file.name());
            lv_obj_add_event_cb(btn, massStorageEventHandler, LV_EVENT_CLICKED, (void *)file.name());
            Serial.print (file.name());
            time_t t = file.getLastWrite();
            struct tm *tmstruct = localtime(&t);
            Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, ( tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
            if (levels) {
                listDir(fs, file.path(), levels - 1);
            }
        } else {
            lv_obj_t *btn = lv_list_add_btn(massStorageList, LV_SYMBOL_FILE, file.name());
            lv_obj_add_event_cb(btn, massStorageEventHandler, LV_EVENT_CLICKED, (void *)file.name());
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.print(file.size());
            time_t t = file.getLastWrite();
            struct tm *tmstruct = localtime(&t);
            Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, ( tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
        }
        file = root.openNextFile();
    }
    SPI_UNLOCK;
}


void massStorageEventHandler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    char filename[256];
    if (code == LV_EVENT_CLICKED) {

        Serial.printf("Clicked: %s ", lv_list_get_btn_text(massStorageList, obj));
        snprintf(filename, 256, "A:/%s",  lv_list_get_btn_text(massStorageList, obj));
        Serial.printf("open -> %s\n", filename);

        lv_img_header_t header;

        if (lv_img_decoder_get_info(filename, &header) == LV_RES_OK) {

            if (imageView) {
                lv_obj_del(imageView);
            }

            imageView = lv_obj_create(lv_scr_act());
            uint16_t w = header.w;
            uint16_t h = header.h;

            lv_obj_set_size(imageView, w, h);
            lv_obj_t *image = lv_img_create(imageView);
            lv_img_set_src(image, filename);
            lv_obj_set_size(image, w, h);
            lv_obj_center(image);

            lv_obj_add_flag(image, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_event_cb(
            image, [](lv_event_t *e) {
                Serial.println("Click !");
                lv_obj_del(( lv_obj_t *)lv_event_get_user_data(e));
                imageView = NULL;
            },
            LV_EVENT_CLICKED, imageView);
        }

    }
}

void massStorageUI(lv_obj_t *parent)
{
    if (!initSD()) {
        Serial.println("SD FAILED!");

        lv_obj_t *cont = lv_obj_create(parent);
        lv_obj_set_style_radius(cont, 0, 0);
        lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
        lv_obj_center(cont);

        lv_obj_t *label1 = lv_label_create(cont);
        lv_label_set_recolor(label1, true);
        lv_obj_set_style_text_font(label1, &lv_font_montserrat_44, 0);
        lv_label_set_text(label1, "#ff0000 No SDCard dected! #");

        lv_obj_center(label1);
        return ;
    }

    massStorageList = lv_list_create(parent);
    lv_obj_set_size(massStorageList, lv_obj_get_width(parent), lv_obj_get_height(parent));
}

static void event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked");
        switch (*(uint8_t *)lv_event_get_user_data(e)) {
        case 0: {
            cameraRun = true;
            Serial.println("Start Camera!");
        }
        break;
        case 1: {
            cameraRun = false;
            lv_draw_label_dsc_t label_dsc;
            lv_draw_label_dsc_init(&label_dsc);
            label_dsc.color = lv_palette_main(LV_PALETTE_ORANGE);
            lv_canvas_draw_text(canvas, 40, 20, 100, &label_dsc, "Camera Stoped!");
            Serial.println("Stop Camera!");
        }
        break;
        case 2:
            capture = true;
            break;
        default:
            break;
        }
    }
}


void cameraUi(lv_obj_t *parent)
{

    if (!hasCamera) {
        Serial.println("No Camera!");
        lv_obj_t *label =  lv_label_create(parent);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_44, 0);
        lv_obj_set_style_text_color(label, lv_color_make(255, 0, 0), 0);
        lv_label_set_text(label, "No Camera dected!");
        lv_obj_center(label);
        return;
    }

    static uint8_t index[3] = {0, 1, 2};
    cbuf =  (lv_color_t *)lv_mem_alloc(CANVAS_WIDTH * CANVAS_HEIGHT * sizeof(lv_color_t));
    lv_memset(cbuf, 0, CANVAS_WIDTH * CANVAS_HEIGHT * sizeof(lv_color_t));
    assert(cbuf);

    canvas = lv_canvas_create(parent);
    lv_canvas_set_buffer(canvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, CANVAS_FORMART );
    lv_obj_center(canvas);

    lv_obj_t *btn = lv_btn_create(canvas);
    lv_obj_set_width(btn, 80);
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, "Start");
    lv_obj_align(btn, LV_ALIGN_RIGHT_MID, -50, -80);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, &index[0]);

    btn = lv_btn_create(canvas);
    lv_obj_set_width(btn, 80);
    label = lv_label_create(btn);
    lv_label_set_text(label, "Stop");
    lv_obj_align(btn, LV_ALIGN_RIGHT_MID, -50, 0);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, &index[1]);

    btn = lv_btn_create(canvas);
    lv_obj_set_width(btn, 80);
    label = lv_label_create(btn);
    lv_label_set_text(label, "Capture");
    lv_obj_align(btn, LV_ALIGN_RIGHT_MID, -50, 80);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, &index[2]);


    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = lv_palette_main(LV_PALETTE_ORANGE);
    lv_canvas_draw_text(canvas, 40, 20, 200, &label_dsc, "Camera Stoped!");
}

//! Power management
bool initPMU()
{
    hasPMU =  PMU.init(Wire, BOARD_I2C_SDA, BOARD_I2C_SCL, SY6970_SLAVE_ADDRESS);
    if (!hasPMU) {
        Serial.println("Failed to find Power management !"); return false;
    } else {

        // Set USB input current limit
        PMU.setInputCurrentLimit(1000);

        // The onboard battery is fully charged at 4.35V
        // Set the charging target voltage, Range:3840 ~ 4608mV ,step:16 mV
        PMU.setChargeTargetVoltage(4352);

        Serial.printf("Get Charge Target Voltage:%u\n", PMU.getChargeTargetVoltage());

        // Set the precharge current , Range: 64mA ~ 1024mA ,step:64mA
        PMU.setPrechargeCurr(64);

        // Set the charging current , Range:0~5056mA ,step:64mA
        PMU.setChargerConstantCurr(640);
        Serial.printf("Get Charge Constant current:%u\n", PMU.getChargerConstantCurr());

        // To obtain voltage data, the ADC must be enabled first
        PMU.enableADCMeasure();

        // Turn off the PMU charging indicator light
        PMU.disableStatLed();

        // For devices that have been connected to the battery, the charging function is turned on by default.
        // PMU.enableCharge();

        // For boards without an external battery, the charging function should be turned off, otherwise the power supply current of the power chip will be limited.
        PMU.disableCharge();

        // Turn off OTG power supply output
        PMU.disableOTG();

        Serial.println("Find Power management");
    }
    return true;
}

//! Capacitive Touch
bool initCapacitiveTouch()
{
    touch.setPins(BOARD_TOUCH_RST, BOARD_TOUCH_IRQ);
    hasTouch = touch.begin(Wire, CST226SE_SLAVE_ADDRESS, BOARD_I2C_SDA, BOARD_I2C_SCL);
    if (!hasTouch) {
        Serial.println("Failed to find Capacitive Touch !"); return false;
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
                    manualOff = true;
                    isBacklightOn = false;
                } else {
                    manualOff = false;
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
    return true;
}

//! Proximity sensor & Light sensor
bool initSensor()
{
    pinMode(BOARD_SENSOR_IRQ, INPUT_PULLUP);
    hasALS = als.begin(Wire, LTR553_SLAVE_ADDRESS, BOARD_I2C_SDA, BOARD_I2C_SCL);
    if (!hasALS) {
        Serial.println("Failed to find Proximity & Light sensor !"); return false;
    } else {
        Serial.println("Find Proximity & Light sensor");

        // Set the ambient light high and low thresholds.
        // If the value exceeds or falls below the set value, an interrupt will be triggered.
        als.setLightSensorThreshold(10, 200);

        // Set the high and low thresholds of the proximity sensor.
        // If the value exceeds or falls below the set value, an interrupt will be triggered.
        // als.setProximityThreshold(10, 30);

        // Controls the Light Sensor N number of times the measurement data is outside the range
        // defined by the upper and lower threshold limits before asserting the interrupt.
        als.setLightSensorPersists(1);

        // Controls the Proximity  N number of times the measurement data is outside the range
        // defined by the upper and lower threshold limits before asserting the interrupt.
        als.setProximityPersists(1);

        /*
        *  ALS_IRQ_ACTIVE_LOW, // INT pin is considered active when it is a logic 0 (default)
        *  ALS_IRQ_ACTIVE_HIGH // INT pin is considered active when it is a logic 1
        * * */
        als.setIRQLevel(SensorLTR553::ALS_IRQ_ACTIVE_LOW);

        /*
        *  ALS_IRQ_ONLY_PS,    // Only PS measurement can trigger interrupt
        *  ALS_IRQ_ONLY_ALS,   // Only ALS measurement can trigger interrupt
        *  ALS_IRQ_BOTH,       // Both ALS and PS measurement can trigger interrupt
        * * * */
        als.enableIRQ(SensorLTR553::ALS_IRQ_BOTH);

        /*
        *   ALS_GAIN_1X  ,   -> 1 lux to 64k lux (default)
        *   ALS_GAIN_2X  ,   -> 0.5 lux to 32k lux
        *   ALS_GAIN_4X  ,   -> 0.25 lux to 16k lux
        *   ALS_GAIN_8X  ,   -> 0.125 lux to 8k lux
        *   ALS_GAIN_48X ,   -> 0.02 lux to 1.3k lux
        *   ALS_GAIN_96X ,   -> 0.01 lux to 600 lux
        * */
        als.setLightSensorGain(SensorLTR553::ALS_GAIN_1X);

        /*
        *   PS_LED_PLUSE_30KHZ,
        *   PS_LED_PLUSE_40KHZ,
        *   PS_LED_PLUSE_50KHZ,
        *   PS_LED_PLUSE_60KHZ,
        *   PS_LED_PLUSE_70KHZ,
        *   PS_LED_PLUSE_80KHZ,
        *   PS_LED_PLUSE_90KHZ,
        *   PS_LED_PLUSE_100KHZ,
        * * * * * * * * * * */
        als.setPsLedPulsePeriod(SensorLTR553::PS_LED_PLUSE_100KHZ);


        /*
        *   PS_LED_DUTY_25,
        *   PS_LED_DUTY_50,
        *   PS_LED_DUTY_75,
        *   PS_LED_DUTY_100,
        * * * */
        als.setPsLedDutyCycle(SensorLTR553::PS_LED_DUTY_100);


        /*
        *   PS_LED_CUR_5MA,
        *   PS_LED_CUR_10MA,
        *   PS_LED_CUR_20MA,
        *   PS_LED_CUR_50MA,
        *   PS_LED_CUR_100MA,
        * * * * * * * */
        als.setPsLedCurrnet(SensorLTR553::PS_LED_CUR_100MA);

        /*
        *   PS_MEAS_RATE_50MS,
        *   PS_MEAS_RATE_70MS,
        *   PS_MEAS_RATE_100MS,
        *   PS_MEAS_RATE_200MS,
        *   PS_MEAS_RATE_500MS,
        *   PS_MEAS_RATE_1000MS,
        *   PS_MEAS_RATE_2000MS,
        *   PS_MEAS_RATE_10MS
        * * * * * * * */
        als.setProximityRate(SensorLTR553::PS_MEAS_RATE_50MS);

        // Number of pulses
        als.setPsLedPulses(1);

        // Enable ambient light sensor
        als.enableLightSensor();

        // Enable proximity sensor
        als.enableProximity();

        // Enable proximity sensor saturation indication
        als.enablePsIndicator();
    }
    return true;
}


/**
 * @brief   !LVGL!
 * @note
 */
void lv_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area,
                   lv_color_t *color_p)
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );
    SPI_LOCK;
    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )&color_p->full, w * h, false );
    tft.endWrite();
    SPI_UNLOCK;
    lv_disp_flush_ready( disp );
}

void lv_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    if (!hasTouch)return;
    uint8_t touched = touch.getPoint(x, y, touch.getSupportTouchPoint());
    if (touched) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = y[0];
        data->point.y = tft.height() - x[0];
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

void lv_helper()
{
    static lv_color_t *frameBuffer;
    static lv_disp_drv_t disp_drv;
    static lv_disp_draw_buf_t draw_buf;

    // Init Display
    tft.init();
    tft.fillScreen(TFT_BLACK);
    tft.setRotation(1);

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
}

void initBackLight()
{
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

bool initSD()
{
    SPI_LOCK;
    SPI.begin(BOARD_SPI_SCK, BOARD_SPI_MISO, BOARD_SPI_MOSI);
    if (!SD.begin(BOARD_SD_CS)) {
        Serial.println("Card Mount Failed");
        SPI_UNLOCK;
        return false;
    }
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
        SPI_UNLOCK;
        return false;
    }
    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC) {
        Serial.println("MMC");
    } else if (cardType == CARD_SD) {
        Serial.println("SDSC");
    } else if (cardType == CARD_SDHC) {
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
    SPI_UNLOCK;
    return true;
}

void WiFiEvent(WiFiEvent_t event)
{
    Serial.printf("[WiFi-event] event: %d\n", event);

    switch (event) {
    case ARDUINO_EVENT_WIFI_READY:
        Serial.println("WiFi interface ready");
        break;
    case ARDUINO_EVENT_WIFI_SCAN_DONE:
        Serial.println("Completed scan for access points");
        break;
    case ARDUINO_EVENT_WIFI_STA_START:
        Serial.println("WiFi client started");
        break;
    case ARDUINO_EVENT_WIFI_STA_STOP:
        Serial.println("WiFi clients stopped");
        break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        Serial.println("Connected to access point");
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        Serial.println("Disconnected from WiFi access point");
        break;
    case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
        Serial.println("Authentication mode of access point has changed");
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        Serial.print("Obtained IP address: ");
        Serial.println(WiFi.localIP());
        configTime(GMT_OFFSET_SEC, DAY_LIGHT_OFFSET_SEC, NTP_SERVER1, NTP_SERVER2);

        if (!vUpdateDateTimeTaskHandler) {
            xTaskCreate(datetimeSyncTask, "sync", 10 * 1024, NULL, 12, &vUpdateDateTimeTaskHandler);
        }
        break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
        Serial.println("Lost IP address and IP address is reset to 0");
        break;
    default: break;
    }
}

void initWiFi()
{
    WiFi.mode(WIFI_STA);
    // Register WiFi event
    WiFi.onEvent(WiFiEvent);
    Serial.println("Begin WiFi");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void datetimeSyncTask(void *ptr)
{
    int httpCode;
    while (1) {
        delay(5000);
        client.setCACert(rootCACertificate);
        if (https.begin(client, GET_TIMEZONE_API)) {
            httpCode = https.GET();
            if (httpCode > 0) {
                Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
                if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                    httpBody = https.getString();
                }
            } else {
                Serial.printf("[HTTPS] GET... failed, error: %s\n",
                              https.errorToString(httpCode).c_str());
                // When the time zone cannot be obtained, the default time zone is used
                httpBody = "none";
            }
            https.end();
        }

        client.stop();

        for (uint32_t i = 0; i < sizeof(zones); i++) {
            if (httpBody == "none") {
                // When the time zone cannot be obtained, the default time zone is used
                httpBody = DEFAULT_TIMEZONE;
                break;
            }
            if (httpBody == zones[i].name) {
                httpBody = zones[i].zones;
                break;
            }
        }
        Serial.println("timezone : " + httpBody);
        setenv("TZ", httpBody.c_str(), 1); // set time zone
        tzset();

        vUpdateDateTimeTaskHandler = NULL;
        // Just run once
        vTaskDelete(NULL);
    }
}

bool initCamera()
{
    cameraConfig.ledc_channel = LEDC_CHANNEL_0;
    cameraConfig.ledc_timer = LEDC_TIMER_0;
    cameraConfig.pin_d0 = CAMERA_PIN_Y2;
    cameraConfig.pin_d1 = CAMERA_PIN_Y3;
    cameraConfig.pin_d2 = CAMERA_PIN_Y4;
    cameraConfig.pin_d3 = CAMERA_PIN_Y5;
    cameraConfig.pin_d4 = CAMERA_PIN_Y6;
    cameraConfig.pin_d5 = CAMERA_PIN_Y7;
    cameraConfig.pin_d6 = CAMERA_PIN_Y8;
    cameraConfig.pin_d7 = CAMERA_PIN_Y9;
    cameraConfig.pin_xclk = CAMERA_PIN_XCLK;
    cameraConfig.pin_pclk = CAMERA_PIN_PCLK;
    cameraConfig.pin_vsync = CAMERA_PIN_VSYNC;
    cameraConfig.pin_href = CAMERA_PIN_HREF;
    cameraConfig.pin_sccb_sda = CAMERA_PIN_SIOD;
    cameraConfig.pin_sccb_scl = CAMERA_PIN_SIOC;
    cameraConfig.pin_pwdn = CAMERA_PIN_PWDN;
    cameraConfig.pin_reset = CAMERA_PIN_RESET;
    cameraConfig.xclk_freq_hz = XCLK_FREQ_HZ;
    cameraConfig.pixel_format = PIXFORMAT_RGB565;

    cameraConfig.frame_size = FRAMESIZE_HVGA;
    cameraConfig.jpeg_quality = 12;
    cameraConfig.fb_count = 2;
    cameraConfig.fb_location = CAMERA_FB_IN_PSRAM;
    cameraConfig.grab_mode = CAMERA_GRAB_WHEN_EMPTY;

    // camera init
    esp_err_t  err =  esp_camera_init(&cameraConfig) ;
    if (err != ESP_OK) {
        hasCamera = false;
        Serial.print("Camera init FAIL,"); Serial.print("err = "); Serial.println(err);
        return false;
    }

    sensor_t *s = esp_camera_sensor_get();
    if (s) {
        Serial.print("camera id:");
        Serial.println(s->id.PID);
        sinfo = esp_camera_sensor_get_info(&(s->id));
        Serial.print("camera model:");
        Serial.println(sinfo->name);
        if (s->id.PID == GC0308_PID) {
            s->set_vflip(s, 0);
            s->set_hmirror(s, 0);
        }
    }

    hasCamera = true;

    return true;
}


void buttonHandler(void *)
{
    while (1) {
        for (uint8_t i = 0; i < BOARD_USER_BTN_NUM; i++) {
            buttons[i].check();
        }
        delay(5);
    }
}

// The event handler for the button.
void handleEvent(AceButton *button, uint8_t eventType, uint8_t buttonState)
{
    // Print out a message for all events.
    Serial.print(F("handleEvent(): eventType: "));
    Serial.print(AceButton::eventName(eventType));
    Serial.print(F("; buttonState: "));
    Serial.println(buttonState);

    uint8_t id = button->getId();

    if (eventType != AceButton::kEventPressed) {
        return;
    }
    switch (id) {
    case 0: {
        //Sleep

        if (hasALS) {
            als.disableLightSensor();
        }
        if (hasTouch) {
            touch.sleep();
        }

        if (hasPMU) {
            PMU.disableOTG();

            PMU.disableStatLed();

            PMU.disableADCMeasure();
        }
        if (hasCamera) {
            esp_camera_deinit();
        }

        // Enter display in sleep mode
        tft.writecommand(0x10);

        Wire.end();

        SPI.end();

        esp_sleep_enable_ext1_wakeup(BIT(16), ESP_EXT1_WAKEUP_ALL_LOW);

        Serial.println("Sleep Start!");

        delay(3000);
        esp_deep_sleep_start();
        Serial.println("This place will never print!");
    }
    break;
    case 1:
        break;
    case 2:
        led_state ? ledcWrite(LEDC_WHITE_CH, 0) : ledcWrite(LEDC_WHITE_CH, 20) ;
        led_state = !led_state;
        break;
    default:
        break;
    }
}

void initButton()
{
    // Initialize Camera LED
    // Adjust the LED duty cycle to save power and heat.
    // If you directly set the LED to HIGH, the heat brought by the LED will be huge,
    // and the current consumption will also be huge.
    ledcAttachPin(CAMERA_WHITH_LED, LEDC_WHITE_CH);
    ledcSetup(LEDC_WHITE_CH, 1000, 8);
    ledcWrite(LEDC_WHITE_CH, 0);
    ledcWrite(LEDC_WHITE_CH, 0);

    for (uint8_t i = 0; i < BOARD_USER_BTN_NUM; i++) {
        // Button uses the built-in pull up register.
        pinMode(buttonsArray[i], INPUT_PULLUP);
        // initialize the corresponding AceButton
        buttons[i].init(buttonsArray[i], HIGH, i);
    }

    // Configure the ButtonConfig with the event handler, and enable all higher
    // level events.
    ButtonConfig *buttonConfig = ButtonConfig::getSystemButtonConfig();
    buttonConfig->setEventHandler(handleEvent);
    buttonConfig->setFeature(ButtonConfig::kFeatureClick);
    buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);
    buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
    buttonConfig->setFeature(ButtonConfig::kFeatureRepeatPress);


    xTaskCreate(buttonHandler, "btn", 2 * 1024, NULL, 10, NULL);
}
