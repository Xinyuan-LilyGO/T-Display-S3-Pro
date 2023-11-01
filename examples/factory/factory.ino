#include <SensorLTR553.hpp>
#include <Wire.h>
#include <WiFi.h>
#include <sntp.h>
#include <XPowersLib.h>
#include "LV_Helper.h"
#include "ui.h"
#include "utilities.h"

static SensorLTR553 als;
static PowersSY6970 PMU;
static WiFiEvent_t event;
static QueueHandle_t wifi_setting_queue;
static EventGroupHandle_t touch_eg;
static bool hasSensor = false, hasPMU = false;
static int autonBrightness = 0;

void lv_auto_brightne_cd(void)
{
    if (!hasSensor)return;
    autonBrightness = autonBrightness == 0 ? 1 : 0;
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
    default: break;
    }
}

bool initSensor()
{

    if (!als.begin(Wire, LTR553_SLAVE_ADDRESS, BOARD_I2C_SDA, BOARD_I2C_SCL)) {
        Serial.println("Failed to find LTR553 - check your wiring!");
        return false;
    }

    Serial.println("Init LTR553 Sensor success!");

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
    return  true;
}

void updatePMU(lv_timer_t *t)
{
    if (!hasPMU)return;
    set_usbAndBattery(PMU.isVbusIn(), PMU.getVbusVoltage(), PMU.getBattVoltage());
}

void updateLightDected(lv_timer_t *t)
{
    if (autonBrightness) {
        bool saturated;
        Serial.print(" ALS: CH1:"); Serial.print(als.getLightSensor(1));
        Serial.print(" -  CH0:"); Serial.print(als.getLightSensor(0));
        Serial.print(" -  PS:"); Serial.print(als.getProximity(&saturated));
        Serial.print(" -  "); Serial.println(saturated ? "PS saturated" : "PS not saturated");
    }
}

void setup()
{
    touch_eg = xEventGroupCreate();
    wifi_setting_queue = xQueueCreate(5, sizeof(uint16_t));

    Serial.begin(115200);

    WiFi.disconnect(true);
    WiFi.onEvent(WiFiEvent);

    const char *ntpServer1 = "pool.ntp.org";
    const char *ntpServer2 = "time.nist.gov";
    const char *tz  = "CST-8";                  //timezone
    configTzTime(tz, ntpServer1, ntpServer2);


    hasPMU =  PMU.init(Wire, BOARD_I2C_SDA, BOARD_I2C_SCL, SY6970_SLAVE_ADDRESS);
    if (!hasPMU) {
        Serial.println("PMU is not online...");
    } else {
        PMU.disableOTG();
        PMU.enableADCMeasure();
        PMU.enableCharge();
    }

    hasSensor = initSensor();

    lv_helper();

    lv_touch_homekey_set_cb(return_home_cd);

    ui_init();

    lv_timer_create(updatePMU, 1000, NULL);

    // TODO:
    // lv_timer_create(updateLightDected, 500, NULL);
}


void loop()
{
    if (xQueueReceive(wifi_setting_queue, &event, 0)) {
        if (event == ARDUINO_EVENT_WIFI_STA_GOT_IP
                || event == ARDUINO_EVENT_WIFI_STA_DISCONNECTED) {
            wifi_connect_cd();
        }
    }
    lv_task_handler();
    delay(2);

}

