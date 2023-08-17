#include <Arduino.h>

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

void setup() {
    Serial.begin(115200);
    Serial.println("touch demo init sta\n");
    delay(10);

    pinMode(TOUCH_RES, OUTPUT);
    digitalWrite(TOUCH_RES, 0);delay(100);
    digitalWrite(TOUCH_RES, 1);delay(100);

    touch_eg = xEventGroupCreate();
    Wire.begin(TOUCH_IICSDA, TOUCH_IICSCL);
    touch.init();

    Serial.println("touch demo init end\n");
}

void loop() {
    if (touch.read()) 
    {
        TP_Point t = touch.getPoint(0);

        printf("x = %d, y = %d\n", t.x, t.y);
    }
    delay(50);
}
