/*
 * @Description:
            T-Display-S3-Pro-MVSRBoard This is an example of recording audio using a digital microphone with 16-bit,
        44.1kHz mono sampling rate.
 * @Author: LILYGO_L
 * @Date: 2023-08-17 15:55:47
 * @LastEditTime: 2024-09-13 21:25:02
 * @License: GPL 3.0
 */

#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"

#define MICROPHONE_RECORD_TIME 10    // 采样时间
#define MICROPHONE_SAMPLE_RATE 44100 // 采样速率
#define MICROPHONE_DATA_BIT 16       // 数据位数
// 一秒采集的字节数量 计算方式：采样速率x声道数量x数据位数/8 (可以通过修改该值来修正录音出现噪点的情况)
#define MICROPHONE_BYTERATE ((MICROPHONE_SAMPLE_RATE * MICROPHONE_NUM_CHANNEL * MICROPHONE_DATA_BIT) / 8)
#define MICROPHONE_WAVE_DATA_SIZE (MICROPHONE_RECORD_TIME * MICROPHONE_BYTERATE) // 采样字节
#define MICROPHONE_NUM_COMMUNICATION_DATA 500                                    // 每次采集的数据量

#define MICROPHONE_NUM_CHANNEL 2                                                   // 声道数量
#define MICROPHONE_BLOCKALIGN ((MICROPHONE_NUM_CHANNEL * MICROPHONE_DATA_BIT) / 8) // 块对齐方式

#define MICROPHONE_HEADER_SIZE 44

#define MICROPHONE_FILE_NAME "/sound.wav"

byte Wave_Header[MICROPHONE_HEADER_SIZE]; // WAVE文件的文件头
char Wave_CommunicationData[MICROPHONE_NUM_COMMUNICATION_DATA];

File file;

std::shared_ptr<Arduino_IIS_DriveBus> IIS_Bus =
    std::make_shared<Arduino_HWIIS>(I2S_NUM_1, MSM261_BCLK, MSM261_WS, MSM261_DOUT);

std::unique_ptr<Arduino_IIS> MSM261(new Arduino_MEMS(IIS_Bus));

void CreateWavHeader(uint8_t *header, int totalDataLen, int longSampleRate, uint8_t channels, int byteRate, uint8_t blockAlign)
{
    header[0] = 'R';
    header[1] = 'I';
    header[2] = 'F';
    header[3] = 'F';
    unsigned int fileSizeMinus8 = totalDataLen + 44 - 8;
    header[4] = (uint8_t)(fileSizeMinus8 & 0xFF);
    header[5] = (uint8_t)((fileSizeMinus8 >> 8) & 0xFF);
    header[6] = (uint8_t)((fileSizeMinus8 >> 16) & 0xFF);
    header[7] = (uint8_t)((fileSizeMinus8 >> 24) & 0xFF);
    header[8] = 'W';
    header[9] = 'A';
    header[10] = 'V';
    header[11] = 'E';
    header[12] = 'f';
    header[13] = 'm';
    header[14] = 't';
    header[15] = ' ';
    header[16] = 0x10; // linear PCM
    header[17] = 0x00;
    header[18] = 0x00;
    header[19] = 0x00;
    header[20] = 0x01; // linear PCM
    header[21] = 0x00;
    // monoral
    header[22] = (uint8_t)(channels & 0xff);
    header[23] = 0x00;
    // 采样率
    header[24] = (uint8_t)(longSampleRate & 0xff);
    header[25] = (uint8_t)((longSampleRate >> 8) & 0xff);
    header[26] = (uint8_t)((longSampleRate >> 16) & 0xff);
    header[27] = (uint8_t)((longSampleRate >> 24) & 0xff);
    // byte Rate
    header[28] = (uint8_t)(byteRate & 0xff);
    header[29] = (uint8_t)((byteRate >> 8) & 0xff);
    header[30] = (uint8_t)((byteRate >> 16) & 0xff);
    header[31] = (uint8_t)((byteRate >> 24) & 0xff);
    // block align
    header[32] = (uint8_t)(blockAlign & 0xff);
    header[33] = 0x00;
    // bits per sample
    header[34] = 0x10;
    header[35] = 0x00;
    header[36] = 'd';
    header[37] = 'a';
    header[38] = 't';
    header[39] = 'a';
    header[40] = (uint8_t)(totalDataLen & 0xFF);
    header[41] = (uint8_t)((totalDataLen >> 8) & 0xFF);
    header[42] = (uint8_t)((totalDataLen >> 16) & 0xFF);
    header[43] = (uint8_t)((totalDataLen >> 24) & 0xFF);
}

void setup()
{
    Serial.begin(115200);

    pinMode(RT9080_EN, OUTPUT);
    digitalWrite(RT9080_EN, HIGH);

    pinMode(MSM261_EN, OUTPUT);
    digitalWrite(MSM261_EN, HIGH);

    while (MSM261->begin(Arduino_IIS_DriveBus::Device_Data_Mode::DATA_IN,
                         MICROPHONE_SAMPLE_RATE, MICROPHONE_DATA_BIT) == false)
    {
        Serial.println("MSM261 initialization fail");
        delay(2000);
    }
    Serial.println("MSM261 initialization successfully");

    SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS); // SPI boots
    while (!SD.begin(SD_CS, SPI, 80000000))
    {
        Serial.printf("SD card initialization failed !\n");
        delay(1000);
    }
    Serial.printf("SD card initialization successfully\n");

    /*
     * @brief 构建WAVE文件的头
     * @param header 构建WAVE的文件
     * @param totalDataLen 需要写入的数据长度
     * @param longSampleRate 采样频率
     * @param channels 声道数量
     * @param byteRate 字节速率 计算方式：采样速率x声道数量x数据位数/8
     * @param blockAlign 块对齐方式 计算方式：声道数量x数据位数/8
    void createWavHeader(byte* header, int totalDataLen, int longSampleRate, uint8_t channels, int byteRate, uint8_t blockAlign);*/
    CreateWavHeader(Wave_Header, MICROPHONE_WAVE_DATA_SIZE, MICROPHONE_SAMPLE_RATE,
                    MICROPHONE_NUM_CHANNEL, MICROPHONE_BYTERATE, MICROPHONE_BLOCKALIGN);

    SD.remove(MICROPHONE_FILE_NAME);
    // 打开文件，如果没有文件就创建文件
    file = SD.open(MICROPHONE_FILE_NAME, FILE_WRITE);

    while (file == 0)
    {
        // 重新打开文件
        file = SD.open(MICROPHONE_FILE_NAME, FILE_WRITE);

        Serial.printf("Fail to open file\n");
        delay(1000);
    }

    // 将wave文件的头写入文件中
    file.write(Wave_Header, MICROPHONE_HEADER_SIZE);
    Serial.printf("Start writing %d second recording data to SD card\n", MICROPHONE_RECORD_TIME);

    for (int j = 0; j < MICROPHONE_WAVE_DATA_SIZE / MICROPHONE_NUM_COMMUNICATION_DATA; ++j)
    {
        MSM261->IIS_Read_Data(Wave_CommunicationData, MICROPHONE_NUM_COMMUNICATION_DATA);

        // 单声道数据处理，当使用单声道时需要处理另一个声道的数据
        // 防止对有效声道的干扰产生杂音，当使用两个麦克风组成双声道时可以屏蔽此步骤。
        for (uint32_t i = 0; i < (MICROPHONE_NUM_COMMUNICATION_DATA >> 2); i++)
        {
            // 合并到右声道
            Wave_CommunicationData[(i << 2)] = 0;
            Wave_CommunicationData[(i << 2) + 1] = 0;

            // 合并到左声道
            // Wave_CommunicationData[(i << 2) + 2] = 0;
            // Wave_CommunicationData[(i << 2) + 3] = 0;
        }
        file.write((uint8_t *)Wave_CommunicationData, MICROPHONE_NUM_COMMUNICATION_DATA);
    }

    file.close();
    Serial.println("Writing SD card data completed");
}

void loop()
{
}
