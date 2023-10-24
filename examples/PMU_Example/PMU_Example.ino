/**
 * @file      PMU_Example.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-09-25
 * @note      USB_CDC_ON_BOOT = Disable  , Messages are output from the QWIIC serial port
 */
#include <XPowersLib.h>
#include "utilities.h"


PowersSY6970 PMU;

uint32_t cycleInterval;

void setup()
{
    Serial.begin(115200);

    bool result = PMU.init(Wire, BOARD_I2C_SDA, BOARD_I2C_SCL, SY6970_SLAVE_ADDRESS);

    if (result == false) {
        while (1) {
            Serial.println("PMU is not online...");
            delay(50);
        }
    }
    // The onboard battery is fully charged at 4.35V
    // Set the charging target voltage, Range:3840 ~ 4608mV ,step:16 mV
    PMU.setChargeTargetVoltage(4352);

    // Set the precharge current , Range: 64mA ~ 1024mA ,step:64mA
    PMU.setPrechargeCurr(64);

    // Set the charging current , Range:0~5056mA ,step:64mA
    PMU.setChargerConstantCurr(320);

    // To obtain voltage data, the ADC must be enabled first
    PMU.enableADCMeasure();


}



void loop()
{

    // SY6970 When VBUS is input, the battery voltage detection will not take effect
    if (millis() > cycleInterval) {

        Serial.println("Sats        VBUS    VBAT   SYS    VbusStatus      String   ChargeStatus     String      TargetVoltage       ChargeCurrent       Precharge       NTCStatus           String");
        Serial.println("            (mV)    (mV)   (mV)   (HEX)                         (HEX)                    (mV)                 (mA)                   (mA)           (HEX)           ");
        Serial.println("--------------------------------------------------------------------------------------------------------------------------------");
        Serial.print(PMU.isVbusIn() ? "Connected" : "Disconnect"); Serial.print("\t");
        Serial.print(PMU.getVbusVoltage()); Serial.print("\t");
        Serial.print(PMU.getBattVoltage()); Serial.print("\t");
        Serial.print(PMU.getSystemVoltage()); Serial.print("\t");
        Serial.print("0x");
        Serial.print(PMU.getBusStatus(), HEX); Serial.print("\t");
        Serial.print(PMU.getBusStatusString()); Serial.print("\t");
        Serial.print("0x");
        Serial.print(PMU.chargeStatus(), HEX); Serial.print("\t");
        Serial.print(PMU.getChargeStatusString()); Serial.print("\t");

        Serial.print(PMU.getChargeTargetVoltage()); Serial.print("\t");
        Serial.print(PMU.getChargerConstantCurr()); Serial.print("\t");
        Serial.print(PMU.getPrechargeCurr()); Serial.print("\t");
        Serial.print(PMU.getNTCStatus()); Serial.print("\t");
        Serial.print(PMU.getNTCStatusString()); Serial.print("\t");

        Serial.println();
        Serial.println();

        cycleInterval = millis() + 1000;
    }

}





