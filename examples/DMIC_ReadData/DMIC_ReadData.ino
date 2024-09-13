/*
 * @Description: 
            T-Display-S3-Pro-MVSRBoard DMIC series microphone testing program
        Send left and right channel signal data on the serial port (value range between 40000 and -40000)
 * @Author: LILYGO_L
 * @Date: 2023-12-20 16:24:06
 * @LastEditTime: 2024-09-13 21:08:20
 * @License: GPL 3.0
 */
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"

#define IIS_SAMPLE_RATE 44100 // 采样速率
#define IIS_DATA_BIT 16       // 数据位数

std::shared_ptr<Arduino_IIS_DriveBus> IIS_Bus =
    std::make_shared<Arduino_HWIIS>(I2S_NUM_1, MSM261_BCLK, MSM261_WS, MSM261_DOUT);

std::unique_ptr<Arduino_IIS> MSM261(new Arduino_MEMS(IIS_Bus));

char IIS_Read_Buff[100];

void setup()
{
    Serial.begin(115200);

    pinMode(RT9080_EN, OUTPUT);
    digitalWrite(RT9080_EN, HIGH);

    pinMode(MSM261_EN, OUTPUT);
    digitalWrite(MSM261_EN, HIGH);

    while (MSM261->begin(Arduino_IIS_DriveBus::Device_Data_Mode::DATA_IN,
                         IIS_SAMPLE_RATE, IIS_DATA_BIT) == false)
    {
        Serial.println("MSM261 initialization fail");
        delay(2000);
    }
    Serial.println("MSM261 initialization successfully");
}

void loop()
{
    if (MSM261->IIS_Read_Data(IIS_Read_Buff, 10) == true)
    {
        // 输出左右声道数据
        Serial.printf("Left: %d\n",(int16_t)((int16_t)IIS_Read_Buff[0] | (int16_t)IIS_Read_Buff[1] << 8));
        Serial.printf("Right: %d\n",(int16_t)((int16_t)IIS_Read_Buff[2] | (int16_t)IIS_Read_Buff[3] << 8));

        // Arduino
        // Serial.println(((int16_t)IIS_Read_Buff[0] | (int16_t)IIS_Read_Buff[1] << 8));
        // Serial.print(",");
        // Serial.print(((int16_t)IIS_Read_Buff[2] | (int16_t)IIS_Read_Buff[3] << 8));
    }
    else
    {
        Serial.printf("Failed to read MSM261 data");
    }

    delay(100);
}
