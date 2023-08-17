#include "pins.h"
#include "Arduino.h"
#include "TFT_eSPI.h"/* Please use the TFT library provided in the library. */
#include "lvgl.h"
#include "ui.h"

#define TOUCH_MODULES_CST_MUTUAL
#include "TouchLib.h"

#include <Wire.h>
#include "WiFi.h"
#include "sntp.h"
#include "esp_timer.h"
#include "driver/ledc.h"
#include "soc/ledc_struct.h"
#include <LTR55x.h>
LTR55x ltr55x;

#include <XPowersLib.h>
PowersSY6970 PMU;

QueueHandle_t wifi_setting_queue;

static EventGroupHandle_t touch_eg;
static const uint16_t screenWidth = 222;
static const uint16_t screenHeight = 480;
TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight);
uint8_t ALS_ADDRESS = 0x5A;
#define LED_PIN       38
#define TOUCH_IICSCL  6
#define TOUCH_IICSDA  5
//#define TOUCH_INT     7
#define TOUCH_RES     21
lv_obj_t *ui_cartext = NULL;
TouchLib touch(Wire, TOUCH_IICSDA, TOUCH_IICSCL, ALS_ADDRESS);

static lv_color_t *buf = NULL;
static lv_color_t *buf1 = NULL;
static lv_disp_draw_buf_t draw_buf;

const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
tm timeinfo;
struct tm get_timeinfo;

#define WAIT 1000
unsigned long targetTime = 0; // Used for testing draw times

time_t now;
char strftime_buf[64] = {0};

void setTime(int sc, int mn, int hr, int dy, int mt, int yr) {
    struct tm t = {0};        // Initalize to all 0's
    t.tm_year = yr - 1900;    // This is year-1900, so 121 = 2021
    t.tm_mon = mt - 1;
    t.tm_mday = dy;
    t.tm_hour = hr;
    t.tm_min = mn;
    t.tm_sec = sc;
    time_t timeSinceEpoch = mktime(&t);
    struct timeval now = { .tv_sec = timeSinceEpoch };
    settimeofday(&now, NULL);
}

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
              return_home_cd();
              touchpad_but = 1;
          }
          return;
      }

      data->state = LV_INDEV_STATE_PR;
      data->point.x = x;
      data->point.y = y;
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

int auto_brightne_t = 0;
void lv_auto_brightne_cd(void)
{
    auto_brightne_t = auto_brightne_t==0?1:0;
}

void scan(void) {
  uint8_t err, addr;
  int nDevices = 0;
  for (addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    err = Wire.endTransmission();
    if (err == 0) {
      Serial.print("I2C device found at address 0x");
      if (addr < 16)
        Serial.print("0");
      Serial.print(addr, HEX);
      Serial.println(" !");
      nDevices++;
    } else if (err == 4) {
      Serial.print("Unknow error at address 0x");
      if (addr < 16)
        Serial.print("0");
      Serial.println(addr, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}
 
extern void lv_demo_benchmark(void);
extern void _ui_screen_change(lv_obj_t * target, lv_scr_load_anim_t fademode, int spd, int delay);

void printLocalTime()
{
    if (!getLocalTime(&get_timeinfo, 2))
    {
        Serial.println("No time available (yet)");
        return;
    }
    if(get_timeinfo.tm_year != 0)
    {
        setTime(get_timeinfo.tm_sec, get_timeinfo.tm_min, get_timeinfo.tm_hour, get_timeinfo.tm_wday, get_timeinfo.tm_mon+1, get_timeinfo.tm_year+1900);
    }

    Serial.println(&get_timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255) {
  // calculate duty, 8191 from 2 ^ 13 - 1
  uint32_t duty = (8191 / valueMax) * min(value, valueMax);

  ledcWrite(channel, duty);
}

void set_sbacklight(uint32_t value)
{
    ledcAnalogWrite(LEDC_CHANNEL_0, value);
}

void WiFiEvent(WiFiEvent_t event)
{
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
            //xQueueSend(wifi_setting_queue, &event, 0);
            Serial.println("Connected to access point");
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            xQueueSend(wifi_setting_queue, &event, 0);
            Serial.println("Disconnected from WiFi access point");
            break;
        case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
            Serial.println("Authentication mode of access point has changed");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            xQueueSend(wifi_setting_queue, &event, 0);
            Serial.print("Obtained IP address: ");
            Serial.println(WiFi.localIP());
            break;
        case ARDUINO_EVENT_WIFI_STA_LOST_IP:
            Serial.println("Lost IP address and IP address is reset to 0");
            break;
        case ARDUINO_EVENT_WPS_ER_SUCCESS:
            Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
            break;
        case ARDUINO_EVENT_WPS_ER_FAILED:
            Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
            break;
        case ARDUINO_EVENT_WPS_ER_TIMEOUT:
            Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
            break;
        case ARDUINO_EVENT_WPS_ER_PIN:
            Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
            break;
        case ARDUINO_EVENT_WIFI_AP_START:
            Serial.println("WiFi access point started");
            break;
        case ARDUINO_EVENT_WIFI_AP_STOP:
            Serial.println("WiFi access point  stopped");
            break;
        case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
            Serial.println("Client connected");
            break;
        case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
            Serial.println("Client disconnected");
            break;
        case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
            Serial.println("Assigned IP address to client");
            break;
        case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
            Serial.println("Received probe request");
            break;
        case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
            Serial.println("AP IPv6 is preferred");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
            Serial.println("STA IPv6 is preferred");
            break;
        case ARDUINO_EVENT_ETH_GOT_IP6:
            Serial.println("Ethernet IPv6 is preferred");
            break;
        case ARDUINO_EVENT_ETH_START:
            Serial.println("Ethernet started");
            break;
        case ARDUINO_EVENT_ETH_STOP:
            Serial.println("Ethernet stopped");
            break;
        case ARDUINO_EVENT_ETH_CONNECTED:
            Serial.println("Ethernet connected");
            break;
        case ARDUINO_EVENT_ETH_DISCONNECTED:
            Serial.println("Ethernet disconnected");
            break;
        case ARDUINO_EVENT_ETH_GOT_IP:
            Serial.println("Obtained IP address");
            break;
        default: break;
    }
}

bool result = false;
bool LTR55x_result = false;
bool touch_result = false;

void setup() {
    touch_eg = xEventGroupCreate();

    // delete old config
    WiFi.disconnect(true);
    // Examples of different ways to register wifi events
    WiFi.onEvent(WiFiEvent);
    sntp_servermode_dhcp(1);    // (optional)
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
    wifi_setting_queue = xQueueCreate(5, sizeof(uint16_t));

    pinMode(TOUCH_RES, OUTPUT);
    digitalWrite(TOUCH_RES, 0);delay(200);
    digitalWrite(TOUCH_RES, 1);delay(200);

    Serial.begin(115200);
    //Serial.println("sta");

    result =  PMU.init(Wire, TOUCH_IICSDA, TOUCH_IICSCL, SY6970_SLAVE_ADDRESS);

    if (result == false) {
        Serial.println("PMU is not online...");
        delay(50);
        Wire.begin(TOUCH_IICSDA, TOUCH_IICSCL);
    }
    else 
    {
        PMU.enableADCMeasure();
    }

    //scan();
    touch_result = touch.init();
    LTR55x_result = ltr55x.begin();

    tft.begin();
    tft.setRotation(0);
    tft.setSwapBytes(true);
    tft.fillScreen(TFT_BLACK);
    delay(20);
    ledcSetup(LEDC_CHANNEL_0, 2000, LEDC_TIMER_13_BIT);
    ledcAttachPin(48, LEDC_CHANNEL_0);
    ledcAnalogWrite(LEDC_CHANNEL_0, 255);

    time(&now);
    setenv("TZ", "CST-8", 1);
    tzset();

    setTime(20,16,16,8,8,2023);

    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);

    lv_init();

    size_t buffer_size =
        sizeof(lv_color_t) * screenWidth * screenHeight;
    buf = (lv_color_t *)ps_malloc(buffer_size);
    buf1 = (lv_color_t *)ps_malloc(buffer_size);
    if (buf == NULL || buf1 == NULL) {
        while (1) {
        Serial.println("buf NULL");
        delay(500);
        }
    }
    lv_disp_draw_buf_init(&draw_buf, buf, buf1, buffer_size);

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
    if(touch_result)
    lv_indev_drv_register(&indev_drv);

    ui_init();

    Serial.println("lv_init end");
}

int con = 0, con1 = 0;
extern void set_ui_Slider2_value(int32_t value);
extern void set_show_time(void);
uint32_t cycleInterval = 0;
WiFiEvent_t event;

void loop() {

    if (xQueueReceive(wifi_setting_queue, &event, 0)) {
        if(event == ARDUINO_EVENT_WIFI_STA_GOT_IP || event == ARDUINO_EVENT_WIFI_STA_DISCONNECTED)
        {
            wifi_connect_cd();
            if (WiFi.status() == WL_CONNECTED)
            {
                printLocalTime();
            }
        }  
    }

    if(auto_brightne_t)
    {
        con++;
        if(con >= 50)
        {
            con = 0;
            if(LTR55x_result)
            {
                uint32_t als_value = ltr55x.get_als_value();
                if(als_value <= 0)
                    als_value = 1;
                ledcAnalogWrite(LEDC_CHANNEL_0, als_value);
                set_ui_Slider2_value(als_value);
                //Serial.printf("als_value=%d ps_value=%d\n", ltr55x.get_als_value(), ltr55x.get_ps_value());
            }
        }
    }

    lv_timer_handler();
    delay(2);

    con1++;
    if(con1 >= 100)
    {
        con1 = 0;
        time(&now);
        localtime_r(&now, &timeinfo);
        set_show_time();
    }

    if (millis() > cycleInterval) {
        if(result != false)
            set_usbAndBattery(PMU.isVbusIn(), PMU.getVbusVoltage(), PMU.getBattVoltage());
        cycleInterval = millis() + 500;
    }
}

