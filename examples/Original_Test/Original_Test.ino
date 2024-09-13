/*
 * @Description: T-Display-S3-Pro-MVSRBoard Original_Test
 * @Author: LILYGO_L
 * @Date: 2023-09-06 10:58:19
 * @LastEditTime: 2024-09-13 22:21:45
 * @License: GPL 3.0
 */

#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "pin_config.h"
#include "Wire.h"
#include "WiFi.h"
#include <HTTPClient.h>
#include "Arduino_DriveBus_Library.h"
#include "Material_16Bit_222x480px.h"
#include "Audio.h"
#include <SPI.h>
#include <SD.h>
#include <FS.h>

// 44.1 KHz
#define IIS_SAMPLE_RATE 44100 // 采样速率
#define IIS_DATA_BIT 16       // 数据位数

#define WIFI_SSID "xinyuandianzi"
#define WIFI_PASSWORD "AA15994823428"
// #define WIFI_SSID "LilyGo-AABB"
// #define WIFI_PASSWORD "xinyuandianzi"

#define WIFI_CONNECT_WAIT_MAX (10000)

#define NTP_SERVER1 "pool.ntp.org"
#define NTP_SERVER2 "time.nist.gov"
#define NTP_SERVER3 "asia.pool.ntp.org"
#define GMT_OFFSET_SEC 8 * 3600 // Time zone setting function, written as 8 * 3600 in East Eighth Zone (UTC/GMT+8:00)
#define DAY_LIGHT_OFFSET_SEC 0  // Fill in 3600 for daylight saving time, otherwise fill in 0

#define SD_FILE_NAME "/music.mp3"
#define SD_FILE_NAME_TEMP "/music_temp.mp3"
#define SD_FILE_SIZE 5434482

// 文件下载链接
// const char *fileDownloadUrl = "https://code.visualstudio.com/docs/?dv=win64user";//vscode
// const char *fileDownloadUrl = "https://dldir1.qq.com/qqfile/qq/PCQQ9.7.17/QQ9.7.17.29225.exe"; // 腾讯CDN加速
// const char *fileDownloadUrl = "https://cd001.www.duba.net/duba/install/packages/ever/kinsthomeui_150_15.exe"; // 金山毒霸
const char *fileDownloadUrl = "http://music.163.com/song/media/outer/url?id=26122999.mp3";
// const char *fileDownloadUrl = "https://github.com/espressif/arduino-esp32/releases/download/3.0.1/esp32-3.0.1.zip";

static bool Wifi_Connection_Flag = true;
static bool SD_Initialization_Flag = false;
static bool SD_File_Download_Check_Flag = false;

static char IIS_Read_Buff[100];

static size_t CycleTime = 0;

static uint8_t Image_Flag = 0;
static int8_t Volume_Value = 0;
static uint8_t Music_Start_Playing_Count = 0;
static bool Music_Start_Playing_Flag = false;

static int8_t Strength_Value = 0;

Audio audio;

Arduino_DataBus *bus = new Arduino_HWSPI(
    LCD_DC /* DC */, LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_MOSI /* MOSI */, LCD_MISO /* MISO */);

Arduino_GFX *gfx = new Arduino_ST7796(
    bus, LCD_RST /* RST */, 0 /* rotation */, true /* IPS */,
    LCD_WIDTH /* width */, LCD_HEIGHT /* height */,
    49 /* col offset 1 */, 0 /* row offset 1 */, 0 /* col_offset2 */, 0 /* row_offset2 */);

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
    std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);

std::shared_ptr<Arduino_IIS_DriveBus> IIS_Bus =
    std::make_shared<Arduino_HWIIS>(I2S_NUM_1, MSM261_BCLK, MSM261_WS, MSM261_DOUT);

std::unique_ptr<Arduino_IIS> MSM261(new Arduino_MEMS(IIS_Bus));

void Arduino_IIC_Touch_Interrupt(void);
void Arduino_IIC_RTC_Interrupt(void);

std::unique_ptr<Arduino_IIC> CST226SE(new Arduino_CST2xxSE(IIC_Bus, CST226SE_DEVICE_ADDRESS,
                                                           TOUCH_RST, TOUCH_INT, Arduino_IIC_Touch_Interrupt));

std::unique_ptr<Arduino_IIC> SY6970(new Arduino_SY6970(IIC_Bus, SY6970_DEVICE_ADDRESS,
                                                       DRIVEBUS_DEFAULT_VALUE, DRIVEBUS_DEFAULT_VALUE));

std::unique_ptr<Arduino_IIC> PCF85063(new Arduino_PCF85063(IIC_Bus, PCF85063_DEVICE_ADDRESS,
                                                           DRIVEBUS_DEFAULT_VALUE, PCF85063_INT, Arduino_IIC_RTC_Interrupt));

void Arduino_IIC_Touch_Interrupt(void)
{
    CST226SE->IIC_Interrupt_Flag = true;
}
void Arduino_IIC_RTC_Interrupt(void)
{
    PCF85063->IIC_Interrupt_Flag = true;
}

void SD_Test(void)
{
    uint8_t SelfLocking_Flag = 0;
    uint8_t cardType = 0;
    uint64_t cardSize = 0;
    uint8_t numSectors = 0;

    while (!SD.begin(SD_CS, SPI) && SelfLocking_Flag != 3) // SD boots
    {
        SelfLocking_Flag++;
        Serial.println("   Detecting SD card");
        gfx->printf("\n\n   Detecting SD card\n");

        Serial.println("   SD card initialization failed !");
        gfx->printf("   SD card initialization failed !\n");
        delay(100);

        Serial.println(".");
        gfx->printf("   .");
        delay(100);

        Serial.println(".");
        gfx->printf(".");
        delay(100);

        Serial.println(".");
        gfx->printf(".");
        delay(100);

        Serial.println(".");
        gfx->printf(".");
        delay(100);

        Serial.println(".");
        gfx->printf(".");
        delay(100);

        Serial.println(".");
        gfx->printf(".");
        delay(100);

        gfx->printf("\n");
    }
    if (SelfLocking_Flag < 3)
    {
        delay(50); // Wait for the SD card
        Serial.println("SD card initialization successful !");
        gfx->printf("\n\nSD card initialization successful !\n");
        SD_Initialization_Flag = true;
        delay(100);

        cardType = SD.cardType();
        cardSize = SD.cardSize() / (1024 * 1024);
        numSectors = SD.numSectors();
        switch (cardType)
        {
        case CARD_NONE:
            Serial.println("No SD card attached");
            gfx->printf("No SD card attached\n");
            gfx->printf("\n\n\nSD card test failed\n");
            delay(100);

            break;
        case CARD_MMC:
            Serial.print("SD Card Type: ");
            Serial.println("MMC");
            Serial.printf("SD Card Size: %lluMB\n", cardSize);
            gfx->printf("SD Card Type: MMC\n   SD Card Size: %lluMB\n", cardSize);
            gfx->printf("\n\n\nSD card test successful\n");
            delay(100);

            break;
        case CARD_SD:
            Serial.print("SD Card Type: ");
            Serial.println("SDSC");
            Serial.printf("SD Card Size: %lluMB\n", cardSize);
            gfx->printf("SD Card Type: SDSC\n   SD Card Size: %lluMB\n", cardSize);
            gfx->printf("\n\n\nSD card tested successful\n");
            delay(100);

            break;
        case CARD_SDHC:
            Serial.print("SD Card Type: ");
            Serial.println("SDHC");
            Serial.printf("SD Card Size: %lluMB\n", cardSize);
            gfx->printf("SD Card Type: SDHC\n   SD Card Size: %lluMB\n", cardSize);
            gfx->printf("\n\n\nSD card tested successful\n");
            delay(100);

            break;
        default:
            Serial.println("UNKNOWN");
            gfx->printf("UNKNOWN");
            gfx->printf("\n\n\nSD card test failed\n");
            delay(100);

            break;
        }
    }
    else
    {
        gfx->printf("\n\n\nSD card test failed\n");
        SD_Initialization_Flag = false;
    }
    SD.end();
}

void Vibration_Start(void)
{
    ledcWrite(2, 255);
    delay(200);
    ledcWrite(2, 0);
}

void Wifi_STA_Test(void)
{
    String text;
    int wifi_num = 0;

    gfx->fillScreen(WHITE);
    gfx->setCursor(0, 0);
    gfx->setTextSize(1);
    gfx->setTextColor(BLACK);

    Serial.println("\nScanning wifi");
    gfx->printf("\n\n\nScanning wifi\n");
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    wifi_num = WiFi.scanNetworks();
    if (wifi_num == 0)
    {
        text = "\nWiFi scan complete !\nNo wifi discovered.\n";
    }
    else
    {
        text = "\nWiFi scan complete !\n";
        text += wifi_num;
        text += " wifi discovered.\n\n";

        for (int i = 0; i < wifi_num; i++)
        {
            text += (i + 1);
            text += ": ";
            text += WiFi.SSID(i);
            text += " (";
            text += WiFi.RSSI(i);
            text += ")";
            text += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " \n" : "*\n";
            delay(10);
        }
    }

    Serial.println(text);
    gfx->println(text);

    delay(3000);
    text.clear();
    gfx->fillScreen(WHITE);
    gfx->setCursor(0, 50);

    Serial.print("Connecting to ");
    gfx->printf("Connecting to\n");

    Serial.print(WIFI_SSID);
    gfx->printf("%s", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    uint32_t last_tick = millis();

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        gfx->printf(".");
        delay(100);

        if (millis() - last_tick > WIFI_CONNECT_WAIT_MAX)
        {
            Wifi_Connection_Flag = false;
            break;
        }
    }

    if (Wifi_Connection_Flag == true)
    {
        Serial.print("\nThe connection was successful ! \nTakes ");
        gfx->printf("\nThe connection was successful ! \nTakes ");
        Serial.print(millis() - last_tick);
        gfx->print(millis() - last_tick);
        Serial.println(" ms\n");
        gfx->printf(" ms\n");

        gfx->setTextColor(PURPLE);
        gfx->printf("\nWifi test passed!");
    }
    else
    {
        gfx->setTextColor(RED);
        gfx->printf("\nWifi test error!\n");
    }
}

void WIFI_HTTP_Download_SD_File(void)
{
    // 初始化HTTP客户端
    HTTPClient http;
    http.begin(fileDownloadUrl);
    // 获取重定向的URL
    const char *headerKeys[] = {"Location"};
    http.collectHeaders(headerKeys, 1);

    // 记录下载开始时间
    size_t startTime = millis();
    // 无用时间
    size_t uselessTime = 0;

    // 发起GET请求
    int httpCode = http.GET();

    while (httpCode == HTTP_CODE_MOVED_PERMANENTLY || httpCode == HTTP_CODE_FOUND)
    {
        String newUrl = http.header("Location");
        Serial.printf("Redirecting to: %s\n", newUrl.c_str());
        http.end(); // 关闭旧的HTTP连接

        // 使用新的URL重新发起GET请求
        http.begin(newUrl);
        httpCode = http.GET();
    }

    if (httpCode == HTTP_CODE_OK)
    {
        // 获取文件大小
        size_t fileSize = http.getSize();
        Serial.printf("Starting file download...\n");
        Serial.printf("file size: %f MB\n", fileSize / 1024.0 / 1024.0);
        gfx->printf("\n\n\nStarting file download...\n");
        gfx->printf("file size: %f MB\n", fileSize / 1024.0 / 1024.0);

        // SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
        if (SD.begin(SD_CS, SPI, 40000000U) == true)
        {
            SD_Initialization_Flag = true;
            SD.remove(SD_FILE_NAME_TEMP);

            File file;
            // 打开文件，如果没有文件就创建文件
            file = SD.open(SD_FILE_NAME_TEMP, FILE_WRITE);

            if (file != 0)
            {
                // 读取HTTP响应
                WiFiClient *stream = http.getStreamPtr();

                size_t temp_count_s = 0;
                size_t temp_fileSize = fileSize;
                // uint8_t *buf_1 = (uint8_t *)heap_caps_malloc(64 * 1024, MALLOC_CAP_SPIRAM);
                uint8_t buf_1[4096] = {0};
                CycleTime = millis() + 3000; // 开始计时
                while (http.connected() && (temp_fileSize > 0 || temp_fileSize == -1))
                {
                    // 获取可用数据的大小
                    size_t availableSize = stream->available();
                    if (availableSize)
                    {
                        // temp_fileSize -= stream->read(buf_1, min(availableSize, (size_t)(64 * 1024)));
                        temp_fileSize -= stream->read(buf_1, min(availableSize, (size_t)(4096)));

                        // file.write(buf_1, min(availableSize, (size_t)(64 * 1024)));
                        file.write(buf_1, min(availableSize, (size_t)(4096)));

                        if (millis() > CycleTime)
                        {
                            size_t temp_time_1 = millis();
                            temp_count_s += 3;
                            Serial.printf("Download speed: %f KB/s\n", ((fileSize - temp_fileSize) / 1024.0) / temp_count_s);
                            Serial.printf("Remaining file size: %f MB\n\n", temp_fileSize / 1024.0 / 1024.0);

                            gfx->fillRect(0, 60, 200, 68, WHITE);
                            gfx->setCursor(0, 60);
                            gfx->printf("Speed: %f KB/s\n", ((fileSize - temp_fileSize) / 1024.0) / temp_count_s);
                            gfx->printf("Size: %f MB\n\n", temp_fileSize / 1024.0 / 1024.0);

                            CycleTime = millis() + 3000;
                            size_t temp_time_2 = millis();

                            uselessTime = uselessTime + (temp_time_2 - temp_time_1);
                        }
                    }
                    // delay(1);
                }

                // 关闭HTTP客户端
                http.end();

                // 记录下载结束时间并计算总花费时间
                size_t endTime = millis();

                file.close();
                file = SD.open(SD_FILE_NAME_TEMP);
                size_t temp_file_wifi_download_size = 0;
                if (file != 0)
                {
                    temp_file_wifi_download_size = file.size();
                }
                file.close();
                SD.end();
                Serial.println("Writing SD card data completed");

                Serial.printf("Download completed!\n");
                Serial.printf("Total download time: %f s\n", (endTime - startTime - uselessTime) / 1000.0);
                Serial.printf("Average download speed: %f KB/s\n", (fileSize / 1024.0) / ((endTime - startTime - uselessTime) / 1000.0));

                gfx->printf("Completed!\n");
                gfx->printf("Time: %f s\n", (endTime - startTime - uselessTime) / 1000.0);
                gfx->printf("Speed: %f KB/s\n", (fileSize / 1024.0) / ((endTime - startTime - uselessTime) / 1000.0));

                if (temp_file_wifi_download_size == SD_FILE_SIZE)
                {
                    SD_File_Download_Check_Flag = true;

                    Serial.printf("\nFile verification successful\n");
                    gfx->printf("\nFile verification successful\n");
                }
                else
                {
                    SD_File_Download_Check_Flag = false;
                    Serial.printf("\nFile verification failed\n");
                    gfx->printf("\nFile verification failed\n");
                }

                Serial.printf("Original file size: %d Bytes\n", SD_FILE_SIZE);
                Serial.printf("Wifi download size: %d Bytes\n", temp_file_wifi_download_size);
                gfx->printf("Original file size: %d Bytes\n", SD_FILE_SIZE);
                gfx->printf("Wifi download size: %d Bytes\n", temp_file_wifi_download_size);
            }
            else
            {
                Serial.printf("Fail to open file\n");
                gfx->printf("Fail to open file\n\n");
            }
        }
        else
        {
            SD_Initialization_Flag = false;
            Serial.printf("SD card initialization failed\n");
            gfx->printf("SD card initialization failed\n\n");
        }
    }
    else
    {
        Serial.printf("Failed to download\n");
        Serial.printf("Error httpCode: %d \n", httpCode);

        gfx->printf("Failed to download\n");
        gfx->printf("Error httpCode: %d \n\n", httpCode);
    }
}

void PrintLocalTime(void)
{
    struct tm timeinfo;
    uint32_t temp_buf = 0;
    bool temp_buf_2 = true;

    while (getLocalTime(&timeinfo) == false)
    {
        Serial.printf("\n.");
        gfx->printf("\n.");
        configTime(GMT_OFFSET_SEC, DAY_LIGHT_OFFSET_SEC, NTP_SERVER1, NTP_SERVER2, NTP_SERVER3);
        temp_buf++;
        if (temp_buf > 5)
        {
            temp_buf_2 = false;
            break;
        }
    }
    if (temp_buf_2 == false)
    {
        Serial.println("Failed to obtain time");
        gfx->setCursor(80, 200);
        gfx->setTextColor(RED);
        gfx->print("Failed to obtain time!");
        return;
    }

    Serial.println("Get time success");
    Serial.println(&timeinfo, "%A,%B %d %Y %H:%M:%S"); // Format Output
    gfx->setCursor(80, 200);
    gfx->setTextColor(PURPLE);
    gfx->print(&timeinfo, " %Y");
    gfx->setCursor(80, 215);
    gfx->print(&timeinfo, "%B %d");
    gfx->setCursor(80, 230);
    gfx->print(&timeinfo, "%H:%M:%S");
}

void GFX_Print_Play_Progress(void)
{
    gfx->setTextSize(2);
    gfx->setCursor(35, 270);

    gfx->fillRect(35, 270, 182, 50, WHITE);
    gfx->printf("%d:%d", audio.getAudioCurrentTime() / 60, audio.getAudioCurrentTime() % 60);

    gfx->setCursor(100, 270);
    gfx->printf("->");

    gfx->setCursor(145, 270);
    gfx->printf("%d:%d", audio.getAudioFileDuration() / 60, audio.getAudioFileDuration() % 60);
}

void GFX_Print_RTC_Info_Loop()
{
    gfx->fillRect(0, 0, LCD_WIDTH, LCD_HEIGHT / 2, WHITE);
    gfx->setTextSize(1);
    gfx->setTextColor(BLACK);

    gfx->setCursor(20, 50);
    gfx->printf("RTC Information");

    gfx->setCursor(20, 65);
    gfx->printf("PCF85063  Weekday: %s\n",
                PCF85063->IIC_Read_Device_State(PCF85063->Arduino_IIC_RTC::Status_Information::RTC_WEEKDAYS_DATA).c_str());
    gfx->setCursor(20, 80);
    gfx->printf("PCF85063  Year: %d\n",
                (int32_t)PCF85063->IIC_Read_Device_Value(PCF85063->Arduino_IIC_RTC::Value_Information::RTC_YEARS_DATA));
    gfx->setCursor(20, 95);
    gfx->printf("PCF85063 Date: %d.%d\n",
                (int32_t)PCF85063->IIC_Read_Device_Value(PCF85063->Arduino_IIC_RTC::Value_Information::RTC_MONTHS_DATA),
                (int32_t)PCF85063->IIC_Read_Device_Value(PCF85063->Arduino_IIC_RTC::Value_Information::RTC_DAYS_DATA));
    gfx->setCursor(20, 110);
    gfx->printf("PCF85063 Time: %d:%d:%d\n",
                (int32_t)PCF85063->IIC_Read_Device_Value(PCF85063->Arduino_IIC_RTC::Value_Information::RTC_HOURS_DATA),
                (int32_t)PCF85063->IIC_Read_Device_Value(PCF85063->Arduino_IIC_RTC::Value_Information::RTC_MINUTES_DATA),
                (int32_t)PCF85063->IIC_Read_Device_Value(PCF85063->Arduino_IIC_RTC::Value_Information::RTC_SECONDS_DATA));
}

void GFX_Print_RTC_Switch_Info()
{
    gfx->fillRect(LCD_WIDTH / 4, LCD_HEIGHT - (LCD_HEIGHT / 3) - 30, LCD_WIDTH / 2, 50, MAROON);
    gfx->drawRect(LCD_WIDTH / 4, LCD_HEIGHT - (LCD_HEIGHT / 3) - 30, LCD_WIDTH / 2, 50, RED);
    gfx->setTextSize(1);
    gfx->setTextColor(WHITE);

    gfx->setCursor(LCD_WIDTH / 4 + 30, LCD_HEIGHT - (LCD_HEIGHT / 3) - 40 + 30);
    gfx->printf("RTC Reset");
}

void GFX_Print_Touch_Info_Loop()
{
    int32_t touch_x_1 = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH1_COORDINATE_X);
    int32_t touch_y_1 = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH1_COORDINATE_Y);
    int32_t touch_x_2 = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH2_COORDINATE_X);
    int32_t touch_y_2 = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH2_COORDINATE_Y);
    int32_t touch_x_3 = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH3_COORDINATE_X);
    int32_t touch_y_3 = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH3_COORDINATE_Y);
    int32_t touch_x_4 = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH4_COORDINATE_X);
    int32_t touch_y_4 = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH4_COORDINATE_Y);
    int32_t touch_x_5 = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH5_COORDINATE_X);
    int32_t touch_y_5 = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH5_COORDINATE_Y);
    uint8_t fingers_number = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

    gfx->fillRect(0, 0, LCD_WIDTH, LCD_HEIGHT / 2, WHITE);
    gfx->setTextSize(1);
    gfx->setTextColor(BLACK);

    gfx->setCursor(20, 50);
    gfx->printf("ID: %#X ", (int32_t)CST226SE->IIC_Device_ID());

    gfx->setCursor(20, 65);
    gfx->printf("Fingers Number:%d ", fingers_number);

    gfx->setCursor(20, 80);
    gfx->printf("Touch X1:%d Y1:%d ", touch_x_1, touch_y_1);
    gfx->setCursor(20, 95);
    gfx->printf("Touch X2:%d Y2:%d ", touch_x_2, touch_y_2);
    gfx->setCursor(20, 110);
    gfx->printf("Touch X3:%d Y3:%d ", touch_x_3, touch_y_3);
    gfx->setCursor(20, 125);
    gfx->printf("Touch X4:%d Y4:%d ", touch_x_4, touch_y_4);
    gfx->setCursor(20, 140);
    gfx->printf("Touch X5:%d Y5:%d ", touch_x_5, touch_y_5);
}

void GFX_Print_Voice_Speaker_Info_Loop(int16_t left_channel, int16_t right_channel)
{
    gfx->fillRect(30, 30, 130, 30, WHITE);
    gfx->setTextSize(1);
    gfx->setTextColor(BLACK);

    gfx->setCursor(30, 30);
    gfx->printf("Voice Data:");

    gfx->setCursor(30, 40);
    gfx->printf("Left Channel:%d ", left_channel);

    gfx->setCursor(30, 50);
    gfx->printf("Right Channel:%d", right_channel);
}

void GFX_Print_Time_Info_Loop()
{
    gfx->fillRoundRect(35, 35, 152, 95, 10, WHITE);

    if (Wifi_Connection_Flag == true)
    {
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo, 1000))
        {
            Serial.println("Failed to obtain time");
            gfx->setCursor(50, 45);
            gfx->setTextColor(RED);
            gfx->setTextSize(1);
            gfx->print("Time error");
            return;
        }
        Serial.println("Get time success");
        Serial.println(&timeinfo, "%A,%B %d %Y %H:%M:%S"); // Format Output
        gfx->setCursor(50, 45);
        gfx->setTextColor(ORANGE);
        gfx->setTextSize(1);
        gfx->print(&timeinfo, " %Y");
        gfx->setCursor(50, 60);
        gfx->print(&timeinfo, "%B %d");
        gfx->setCursor(50, 75);
        gfx->print(&timeinfo, "%H:%M:%S");
    }
    else
    {
        gfx->setCursor(50, 45);
        gfx->setTextSize(2);
        gfx->setTextColor(RED);
        gfx->print("Network error");
    }

    gfx->setCursor(50, 90);
    gfx->printf("SYS Time:%d", (uint32_t)millis() / 1000);
}

void GFX_Print_1()
{
    gfx->fillRect(20, LCD_HEIGHT - (LCD_HEIGHT / 4), 80, 40, ORANGE);
    gfx->drawRect(20, LCD_HEIGHT - (LCD_HEIGHT / 4), 80, 40, PURPLE);
    gfx->fillRect(120, LCD_HEIGHT - (LCD_HEIGHT / 4), 80, 40, PURPLE);
    gfx->drawRect(120, LCD_HEIGHT - (LCD_HEIGHT / 4), 80, 40, ORANGE);
    gfx->setTextSize(1);
    gfx->setTextColor(WHITE);
    gfx->setCursor(35, LCD_HEIGHT - (LCD_HEIGHT / 4) + 15);
    gfx->printf("Try Again");
    gfx->setCursor(135, LCD_HEIGHT - (LCD_HEIGHT / 4) + 15);
    gfx->printf("Next Test");
}

void GFX_Print_TEST(String s)
{
    gfx->fillScreen(WHITE);
    gfx->setCursor(LCD_WIDTH / 4 + 20, LCD_HEIGHT / 4);
    gfx->setTextSize(3);
    gfx->setTextColor(MAROON);
    gfx->printf("TEST");

    gfx->setCursor(20, LCD_HEIGHT / 4 + 40);
    gfx->setTextSize(2);
    gfx->setTextColor(BLACK);
    gfx->print(s);

    gfx->setCursor(LCD_WIDTH / 2, LCD_HEIGHT / 2);
    gfx->setTextSize(4);
    gfx->setTextColor(RED);
    gfx->printf("3");
    delay(1000);
    gfx->fillRect(LCD_WIDTH / 2, LCD_HEIGHT / 2, LCD_WIDTH / 2 + 40, LCD_HEIGHT / 2 + 40, WHITE);
    gfx->setCursor(LCD_WIDTH / 2, LCD_HEIGHT / 2);
    gfx->printf("2");
    delay(1000);
    gfx->fillRect(LCD_WIDTH / 2, LCD_HEIGHT / 2, LCD_WIDTH / 2 + 40, LCD_HEIGHT / 2 + 40, WHITE);
    gfx->setCursor(LCD_WIDTH / 2, LCD_HEIGHT / 2);
    gfx->printf("1");
    delay(1000);
}

void GFX_Print_FINISH()
{
    gfx->setCursor(LCD_WIDTH / 4, LCD_HEIGHT / 4);
    gfx->setTextSize(3);
    gfx->setTextColor(ORANGE);
    gfx->printf("FINISH");
}

void GFX_Print_START()
{
    gfx->setCursor(150, 220);
    gfx->setTextSize(6);
    gfx->setTextColor(RED);
    gfx->printf("START");
}

void GFX_Print_Volume()
{
    gfx->setCursor(LCD_WIDTH / 4 + 5, LCD_WIDTH / 4 + 40);
    gfx->setTextSize(3);
    gfx->setTextColor(ORANGE);
    gfx->printf("Volume");
}

void GFX_Print_Music_Volume_Value()
{
    gfx->fillRect(30, LCD_HEIGHT / 3, 50, 50, MAROON);
    gfx->drawRect(30, LCD_HEIGHT / 3, 50, 50, PURPLE);
    gfx->fillRect(140, LCD_HEIGHT / 3, 50, 50, MAROON);
    gfx->drawRect(140, LCD_HEIGHT / 3, 50, 50, PURPLE);
    gfx->setTextSize(2);
    gfx->setTextColor(WHITE);
    gfx->setCursor(50, (LCD_HEIGHT / 3) + 17);
    gfx->printf("-");
    gfx->setCursor(160, (LCD_HEIGHT / 3) + 17);
    gfx->printf("+");

    gfx->fillRect(LCD_WIDTH / 2 - 13, LCD_HEIGHT / 3 + 13, 35, 35, WHITE);
    gfx->setCursor(LCD_WIDTH / 2 - 13, LCD_HEIGHT / 3 + 13);
    gfx->setTextSize(3);
    gfx->setTextColor(RED);
    gfx->print(audio.getVolume());
}

void GFX_Print_Strength()
{
    gfx->setCursor(LCD_WIDTH / 4 - 15, LCD_WIDTH / 4 + 40);
    gfx->setTextSize(3);
    gfx->setTextColor(ORANGE);
    gfx->printf("Strength");
}

void GFX_Print_Motor_Strength_Value()
{
    gfx->fillRect(30, LCD_HEIGHT / 3, 50, 50, MAROON);
    gfx->drawRect(30, LCD_HEIGHT / 3, 50, 50, PURPLE);
    gfx->fillRect(140, LCD_HEIGHT / 3, 50, 50, MAROON);
    gfx->drawRect(140, LCD_HEIGHT / 3, 50, 50, PURPLE);
    gfx->setTextSize(2);
    gfx->setTextColor(WHITE);
    gfx->setCursor(50, (LCD_HEIGHT / 3) + 17);
    gfx->printf("-");
    gfx->setCursor(160, (LCD_HEIGHT / 3) + 17);
    gfx->printf("+");

    gfx->fillRect(LCD_WIDTH / 2 - 13, LCD_HEIGHT / 3 + 13, 35, 35, WHITE);
    gfx->setCursor(LCD_WIDTH / 2 - 13, LCD_HEIGHT / 3 + 13);
    gfx->setTextSize(3);
    gfx->setTextColor(RED);
    gfx->print(Strength_Value);
}

void GFX_Print_Play_Failed()
{
    gfx->setCursor(20, 30);
    gfx->setTextSize(1);
    gfx->setTextColor(RED);
    gfx->printf("Play failed\n");
}

void Original_Test_1()
{
    GFX_Print_TEST("1.Touch Test");

    gfx->fillScreen(WHITE);

    GFX_Print_Touch_Info_Loop();

    GFX_Print_1();
}

void Original_Test_2()
{
    GFX_Print_TEST("2.RTC Test");

    gfx->fillScreen(WHITE);

    GFX_Print_RTC_Switch_Info();

    GFX_Print_1();
}

void Original_Test_3()
{
    GFX_Print_TEST("3.MSM261 Test");

    gfx->fillScreen(WHITE);

    gfx->setTextSize(1);
    gfx->setTextColor(BLACK);

    gfx->setCursor(20, 50);
    gfx->printf("MSM261 Information");

    // gfx->setCursor(30, 40);
    // gfx->setTextColor(RED);
    // gfx->printf("Please speak into");

    // gfx->setCursor(30, 50);
    // gfx->printf("the microphone");

    GFX_Print_1();
}

void Original_Test_4()
{
    GFX_Print_TEST("4.SD Test");

    gfx->fillScreen(WHITE);
    gfx->setCursor(0, 100);
    gfx->setTextSize(1);
    gfx->setTextColor(BLACK);

    SD_Test();
    // delay(3000);

    // gfx->fillScreen(WHITE);
    // GFX_Print_FINISH();
    GFX_Print_1();
}

void Original_Test_5()
{
    GFX_Print_TEST("5.WIFI STA Test");

    Wifi_STA_Test();

    delay(2000);

    gfx->fillScreen(WHITE);
    gfx->setTextColor(BLACK);
    gfx->setCursor(0, 0);

    if (Wifi_Connection_Flag == true)
    {
        // Obtain and set the time from the network time server
        // After successful acquisition, the chip will use the RTC clock to update the holding time
        configTime(GMT_OFFSET_SEC, DAY_LIGHT_OFFSET_SEC, NTP_SERVER1, NTP_SERVER2, NTP_SERVER3);
        // PrintLocalTime();
        if (SD_Initialization_Flag == true)
        {
            WIFI_HTTP_Download_SD_File();
        }
        else
        {
            gfx->print("\n\n\nSD card initialization failed");
        }
    }
    else
    {
        // gfx->setCursor(80, 200);
        // gfx->setTextColor(RED);
        // gfx->print("Not connected to the network");
    }
    // delay(3000);

    // gfx->fillScreen(WHITE);
    // GFX_Print_FINISH();
    GFX_Print_1();
}

void Original_Test_6()
{
    GFX_Print_TEST("6.SD Music Test");

    gfx->fillScreen(WHITE);
    gfx->setCursor(0, 100);
    gfx->setTextSize(1);
    gfx->setTextColor(PURPLE);
    gfx->println("Trying to play music...");

    while (1)
    {
        if (SD.begin(SD_CS, SPI, 40000000U) == true)
        {
            if (SD_File_Download_Check_Flag == true)
            {
                if (audio.connecttoSD(SD_FILE_NAME_TEMP) == false)
                {
                    Music_Start_Playing_Flag = false;
                }
                else
                {
                    Music_Start_Playing_Flag = true;
                    break;
                }
            }
            else
            {
                if (audio.connecttoSD(SD_FILE_NAME) == false)
                {
                    Music_Start_Playing_Flag = false;
                }
                else
                {
                    Music_Start_Playing_Flag = true;
                    break;
                }
            }
        }
        else
        {
            gfx->printf("SD card initialization failed\n");
        }

        gfx->print(".");

        Music_Start_Playing_Count++;
        if (Music_Start_Playing_Count > 10) // 10秒超时
        {
            Music_Start_Playing_Flag = false;
            break;
        }
        delay(1000);
    }

    Music_Start_Playing_Count = 0;

    while (audio.getAudioCurrentTime() <= 0)
    {
        audio.loop();

        if (millis() > CycleTime)
        {
            Music_Start_Playing_Count++;
            if (Music_Start_Playing_Count > 10) // 10秒后下载超时
            {
                Music_Start_Playing_Flag = false;
                break;
            }
            CycleTime = millis() + 1000;
        }
    }

    gfx->fillScreen(WHITE);
    GFX_Print_1();

    if (Music_Start_Playing_Flag == true)
    {
        GFX_Print_Volume();
        GFX_Print_Music_Volume_Value();
        GFX_Print_Play_Progress();
    }
    else
    {
        GFX_Print_Play_Failed();
    }
}

void Original_Test_7()
{
    GFX_Print_TEST("7.Vibration Motor Test");

    gfx->fillScreen(WHITE);

    GFX_Print_Strength();
    GFX_Print_Motor_Strength_Value();

    GFX_Print_1();
}

void Original_Test_Loop()
{
    Original_Test_1();

    while (1)
    {
        bool temp = false;

        if (CST226SE->IIC_Interrupt_Flag == true)
        {
            CST226SE->IIC_Interrupt_Flag = false;

            int32_t touch_x = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            int32_t touch_y = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
            uint8_t fingers_number = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

            GFX_Print_Touch_Info_Loop();

            if (fingers_number > 0)
            {
                if (touch_x > 20 && touch_x < 100 &&
                    touch_y > LCD_HEIGHT - (LCD_HEIGHT / 4) && touch_y < LCD_HEIGHT - (LCD_HEIGHT / 4) + 40)
                {
                    Vibration_Start();
                    Original_Test_1();
                }
                if (touch_x > 120 && touch_x < 200 &&
                    touch_y > LCD_HEIGHT - (LCD_HEIGHT / 4) && touch_y < LCD_HEIGHT - (LCD_HEIGHT / 4) + 40)
                {
                    Vibration_Start();
                    temp = true;
                }
            }
        }

        if (temp == true)
        {
            break;
        }
    }

    Original_Test_2();

    while (1)
    {
        bool temp = false;

        if (millis() > CycleTime)
        {
            GFX_Print_RTC_Info_Loop();
            CycleTime = millis() + 1000;
        }

        if (CST226SE->IIC_Interrupt_Flag == true)
        {
            CST226SE->IIC_Interrupt_Flag = false;

            int32_t touch_x = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            int32_t touch_y = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
            uint8_t fingers_number = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

            if (fingers_number > 0)
            {
                if (touch_x > 20 && touch_x < 100 &&
                    touch_y > LCD_HEIGHT - (LCD_HEIGHT / 4) && touch_y < LCD_HEIGHT - (LCD_HEIGHT / 4) + 40)
                {
                    Vibration_Start();
                    Original_Test_2();
                }
                if (touch_x > 120 && touch_x < 200 &&
                    touch_y > LCD_HEIGHT - (LCD_HEIGHT / 4) && touch_y < LCD_HEIGHT - (LCD_HEIGHT / 4) + 40)
                {
                    Vibration_Start();
                    temp = true;
                }

                if (touch_x > LCD_WIDTH / 4 && touch_x < LCD_WIDTH / 4 + LCD_WIDTH / 2 &&
                    touch_y > LCD_HEIGHT - (LCD_HEIGHT / 3) - 30 && touch_y < LCD_HEIGHT - (LCD_HEIGHT / 3) - 30 + 50)
                {
                    // 关闭RTC
                    PCF85063->IIC_Write_Device_State(PCF85063->Arduino_IIC_RTC::Device::RTC_CLOCK_RTC,
                                                     PCF85063->Arduino_IIC_RTC::Device_State::RTC_DEVICE_OFF);
                    // 时钟传感器设置秒
                    PCF85063->IIC_Write_Device_Value(PCF85063->Arduino_IIC_RTC::Device_Value::RTC_SET_SECOND_DATA,
                                                     58);
                    // 时钟传感器设置分
                    PCF85063->IIC_Write_Device_Value(PCF85063->Arduino_IIC_RTC::Device_Value::RTC_SET_MINUTE_DATA,
                                                     59);
                    // 时钟传感器设置时
                    PCF85063->IIC_Write_Device_Value(PCF85063->Arduino_IIC_RTC::Device_Value::RTC_SET_HOUR_DATA,
                                                     23);
                    // 时钟传感器设置天
                    PCF85063->IIC_Write_Device_Value(PCF85063->Arduino_IIC_RTC::Device_Value::RTC_SET_DAY_DATA,
                                                     31);
                    // 时钟传感器设置月
                    PCF85063->IIC_Write_Device_Value(PCF85063->Arduino_IIC_RTC::Device_Value::RTC_SET_MONTH_DATA,
                                                     12);
                    // 时钟传感器设置
                    PCF85063->IIC_Write_Device_Value(PCF85063->Arduino_IIC_RTC::Device_Value::RTC_SET_YEAR_DATA,
                                                     99);
                    // 开启RTC
                    PCF85063->IIC_Write_Device_State(PCF85063->Arduino_IIC_RTC::Device::RTC_CLOCK_RTC,
                                                     PCF85063->Arduino_IIC_RTC::Device_State::RTC_DEVICE_ON);

                    Vibration_Start();
                }
            }
        }

        if (temp == true)
        {
            break;
        }
    }

    Original_Test_3();

    while (1)
    {
        bool temp = false;

        if (MSM261->IIS_Read_Data(IIS_Read_Buff, 100) == true)
        {
            gfx->fillRect(0, 70, 222, 110, WHITE);

            gfx->setCursor(20, 85);
            gfx->setTextColor(BLUE);
            gfx->printf("Left:%d", (int16_t)((int16_t)IIS_Read_Buff[0] | (int16_t)IIS_Read_Buff[1] << 8));

            gfx->setCursor(20, 100);
            gfx->printf("Right:%d", (int16_t)((int16_t)IIS_Read_Buff[2] | (int16_t)IIS_Read_Buff[3] << 8));
            delay(100);
        }

        if (CST226SE->IIC_Interrupt_Flag == true)
        {
            CST226SE->IIC_Interrupt_Flag = false;

            int32_t touch_x = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            int32_t touch_y = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
            uint8_t fingers_number = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

            if (fingers_number > 0)
            {
                if (touch_x > 20 && touch_x < 100 &&
                    touch_y > LCD_HEIGHT - (LCD_HEIGHT / 4) && touch_y < LCD_HEIGHT - (LCD_HEIGHT / 4) + 40)
                {
                    Vibration_Start();
                    Original_Test_3();
                }
                if (touch_x > 120 && touch_x < 200 &&
                    touch_y > LCD_HEIGHT - (LCD_HEIGHT / 4) && touch_y < LCD_HEIGHT - (LCD_HEIGHT / 4) + 40)
                {
                    Vibration_Start();
                    temp = true;
                }
            }
        }

        if (temp == true)
        {
            break;
        }
    }

    Original_Test_4();

    while (1)
    {
        bool temp = false;

        if (CST226SE->IIC_Interrupt_Flag == true)
        {
            CST226SE->IIC_Interrupt_Flag = false;

            int32_t touch_x = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            int32_t touch_y = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
            uint8_t fingers_number = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

            if (fingers_number > 0)
            {
                if (touch_x > 20 && touch_x < 100 &&
                    touch_y > LCD_HEIGHT - (LCD_HEIGHT / 4) && touch_y < LCD_HEIGHT - (LCD_HEIGHT / 4) + 40)
                {
                    Vibration_Start();
                    Original_Test_4();
                }
                if (touch_x > 120 && touch_x < 200 &&
                    touch_y > LCD_HEIGHT - (LCD_HEIGHT / 4) && touch_y < LCD_HEIGHT - (LCD_HEIGHT / 4) + 40)
                {
                    Vibration_Start();
                    temp = true;
                }
            }
        }

        if (temp == true)
        {
            break;
        }
    }

    Original_Test_5();

    while (1)
    {
        bool temp = false;

        if (CST226SE->IIC_Interrupt_Flag == true)
        {
            CST226SE->IIC_Interrupt_Flag = false;

            int32_t touch_x = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            int32_t touch_y = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
            uint8_t fingers_number = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

            if (fingers_number > 0)
            {
                if (touch_x > 20 && touch_x < 100 &&
                    touch_y > LCD_HEIGHT - (LCD_HEIGHT / 4) &&
                    touch_y < LCD_HEIGHT - (LCD_HEIGHT / 4) + 40)
                {
                    Vibration_Start();
                    Original_Test_5();
                }
                if (touch_x > 120 && touch_x < 200 &&
                    touch_y > LCD_HEIGHT - (LCD_HEIGHT / 4) &&
                    touch_y < LCD_HEIGHT - (LCD_HEIGHT / 4) + 40)
                {
                    Vibration_Start();
                    temp = true;
                }
            }
        }

        if (temp == true)
        {
            break;
        }
    }

    Original_Test_6();

    while (1)
    {
        bool temp = false;

        if (Music_Start_Playing_Flag == true)
        {
            if (millis() > CycleTime)
            {
                GFX_Print_Play_Progress();
                CycleTime = millis() + 1000;
            }

            audio.loop();
        }

        if (CST226SE->IIC_Interrupt_Flag == true)
        {
            CST226SE->IIC_Interrupt_Flag = false;

            int32_t touch_x = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            int32_t touch_y = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
            uint8_t fingers_number = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

            if (fingers_number > 0)
            {
                if (touch_x > 20 && touch_x < 100 &&
                    touch_y > LCD_HEIGHT - (LCD_HEIGHT / 4) &&
                    touch_y < LCD_HEIGHT - (LCD_HEIGHT / 4) + 40)
                {
                    Vibration_Start();
                    Original_Test_6();
                }
                if (touch_x > 120 && touch_x < 200 &&
                    touch_y > LCD_HEIGHT - (LCD_HEIGHT / 4) &&
                    touch_y < LCD_HEIGHT - (LCD_HEIGHT / 4) + 40)
                {
                    Vibration_Start();
                    temp = true;
                }

                if (Music_Start_Playing_Flag == true)
                {
                    if (touch_x > 30 && touch_x < 80 &&
                        touch_y > LCD_HEIGHT / 3 && touch_y < LCD_HEIGHT / 3 + 50)
                    {
                        Volume_Value--;
                        Vibration_Start();
                        if (Volume_Value < 0)
                        {
                            Volume_Value = 0;
                        }
                    }
                    if (touch_x > 140 && touch_x < 190 &&
                        touch_y > LCD_HEIGHT / 3 && touch_y < LCD_HEIGHT / 3 + 50)
                    {
                        Volume_Value++;
                        Vibration_Start();
                        if (Volume_Value > 21)
                        {
                            Volume_Value = 21;
                        }
                    }

                    audio.setVolume(Volume_Value);
                    GFX_Print_Music_Volume_Value();
                }
            }
        }

        if (temp == true)
        {
            break;
        }
    }

    Original_Test_7();

    while (1)
    {
        bool temp = false;

        if (CST226SE->IIC_Interrupt_Flag == true)
        {
            CST226SE->IIC_Interrupt_Flag = false;

            int32_t touch_x = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            int32_t touch_y = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
            uint8_t fingers_number = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

            if (fingers_number > 0)
            {
                if (touch_x > 20 && touch_x < 100 &&
                    touch_y > LCD_HEIGHT - (LCD_HEIGHT / 4) && touch_y < LCD_HEIGHT - (LCD_HEIGHT / 4) + 40)
                {
                    Vibration_Start();
                    Original_Test_7();
                }
                if (touch_x > 120 && touch_x < 200 &&
                    touch_y > LCD_HEIGHT - (LCD_HEIGHT / 4) && touch_y < LCD_HEIGHT - (LCD_HEIGHT / 4) + 40)
                {
                    Vibration_Start();
                    temp = true;
                }

                if (touch_x > 30 && touch_x < 80 &&
                    touch_y > LCD_HEIGHT / 3 && touch_y < LCD_HEIGHT / 3 + 50)
                {
                    Strength_Value--;
                    if (Strength_Value < 0)
                    {
                        Strength_Value = 0;
                    }
                }
                if (touch_x > 140 && touch_x < 190 &&
                    touch_y > LCD_HEIGHT / 3 && touch_y < LCD_HEIGHT / 3 + 50)
                {
                    Strength_Value++;
                    if (Strength_Value > 20)
                    {
                        Strength_Value = 20;
                    }
                }

                ledcWrite(2, Strength_Value * 10);
                GFX_Print_Motor_Strength_Value();
                delay(200);
            }
        }

        if (temp == true)
        {
            ledcWrite(2, 0);
            break;
        }
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    pinMode(RT9080_EN, OUTPUT);
    digitalWrite(RT9080_EN, HIGH);

    pinMode(MSM261_EN, OUTPUT);
    digitalWrite(MSM261_EN, HIGH);

    pinMode(MAX98357A_SD_MODE, OUTPUT);
    digitalWrite(MAX98357A_SD_MODE, HIGH);

    ledcAttachPin(LCD_BL, 1);
    ledcSetup(1, 2000, 8);
    ledcWrite(1, 255);

    ledcAttachPin(VIBRATINO_MOTOR_PWM, 2);
    ledcSetup(2, 12000, 8);
    ledcWrite(2, 0);

    if (SY6970->begin() == false)
    {
        Serial.println("SY6970 initialization fail");
        delay(2000);
    }
    else
    {
        Serial.println("SY6970 initialization successfully");
    }

    // 开启ADC测量功能
    if (SY6970->IIC_Write_Device_State(SY6970->Arduino_IIC_Power::Device::POWER_DEVICE_ADC_MEASURE,
                                       SY6970->Arduino_IIC_Power::Device_State::POWER_DEVICE_ON) == false)
    {
        Serial.println("SY6970 ADC Measure ON fail");
        delay(2000);
    }
    else
    {
        Serial.println("SY6970 ADC Measure ON successfully");
    }

    // 禁用看门狗定时器喂狗功能
    SY6970->IIC_Write_Device_Value(SY6970->Arduino_IIC_Power::Device_Value::POWER_DEVICE_WATCHDOG_TIMER, 0);
    // 热调节阈值设置为60度
    SY6970->IIC_Write_Device_Value(SY6970->Arduino_IIC_Power::Device_Value::POWER_DEVICE_THERMAL_REGULATION_THRESHOLD, 60);
    // 充电目标电压电压设置为4224mV
    SY6970->IIC_Write_Device_Value(SY6970->Arduino_IIC_Power::Device_Value::POWER_DEVICE_CHARGING_TARGET_VOLTAGE_LIMIT, 4224);
    // 最小系统电压限制为3600mA
    SY6970->IIC_Write_Device_Value(SY6970->Arduino_IIC_Power::Device_Value::POWER_DEVICE_MINIMUM_SYSTEM_VOLTAGE_LIMIT, 3600);
    // 设置OTG电压为5062mV
    SY6970->IIC_Write_Device_Value(SY6970->Arduino_IIC_Power::Device_Value::POWER_DEVICE_OTG_VOLTAGE_LIMIT, 5062);
    // 输入电流限制设置为2100mA
    SY6970->IIC_Write_Device_Value(SY6970->Arduino_IIC_Power::Device_Value::POWER_DEVICE_INPUT_CURRENT_LIMIT, 2100);
    // 快速充电电流限制设置为2112mA
    SY6970->IIC_Write_Device_Value(SY6970->Arduino_IIC_Power::Device_Value::POWER_DEVICE_FAST_CHARGING_CURRENT_LIMIT, 2112);
    // 预充电电流限制设置为192mA
    SY6970->IIC_Write_Device_Value(SY6970->Arduino_IIC_Power::Device_Value::POWER_DEVICE_PRECHARGE_CHARGING_CURRENT_LIMIT, 192);
    // 终端充电电流限制设置为320mA
    SY6970->IIC_Write_Device_Value(SY6970->Arduino_IIC_Power::Device_Value::POWER_DEVICE_TERMINATION_CHARGING_CURRENT_LIMIT, 320);
    // OTG电流限制设置为500mA
    SY6970->IIC_Write_Device_Value(SY6970->Arduino_IIC_Power::Device_Value::POWER_DEVICE_OTG_CHARGING_LIMIT, 500);

    if (CST226SE->begin() == false)
    {
        Serial.println("CST226SE initialization fail");
        delay(2000);
    }
    else
    {
        Serial.println("CST226SE initialization successfully");
    }

    // 目前休眠功能只能进入不能退出 要退出只能系统重置
    // CST226SE->IIC_Write_Device_State(CST226SE->Arduino_IIC_Touch::Device::TOUCH_DEVICE_SLEEP_MODE,
    //                                 CST226SE->Arduino_IIC_Touch::Device_State::TOUCH_DEVICE_ON);

    if (PCF85063->begin() == false)
    {
        Serial.println("PCF85063 initialization fail");
        delay(2000);
    }
    else
    {
        Serial.println("PCF85063 initialization successfully");
    }

    // 设置时间格式为24小时制
    PCF85063->IIC_Write_Device_State(PCF85063->Arduino_IIC_RTC::Device::RTC_CLOCK_TIME_FORMAT,
                                     PCF85063->Arduino_IIC_RTC::Device_Mode::RTC_CLOCK_TIME_FORMAT_24);

    // 关闭时钟输出
    PCF85063->IIC_Write_Device_State(PCF85063->Arduino_IIC_RTC::Device::RTC_CLOCK_OUTPUT_VALUE,
                                     PCF85063->Arduino_IIC_RTC::Device_Mode::RTC_CLOCK_OUTPUT_OFF);

    // 开启RTC
    PCF85063->IIC_Write_Device_State(PCF85063->Arduino_IIC_RTC::Device::RTC_CLOCK_RTC,
                                     PCF85063->Arduino_IIC_RTC::Device_State::RTC_DEVICE_ON);

    if (MSM261->begin(Arduino_IIS_DriveBus::Device_Data_Mode::DATA_IN,
                      IIS_SAMPLE_RATE, IIS_DATA_BIT) == false)
    {
        Serial.println("MSM261 initialization fail");
        delay(2000);
    }
    else
    {
        Serial.println("MSM261 initialization successfully");
    }

    if (audio.setPinout(MAX98357A_BCLK, MAX98357A_LRCLK, MAX98357A_DIN) != true)
    {
        Serial.println("Music driver initialization failed");
        delay(1000);
    }
    else
    {
        Serial.println("Music driver initialization successfully");
    }

    Volume_Value = 3;
    audio.setVolume(Volume_Value); // 0...21,Volume setting

    gfx->begin();
    gfx->fillScreen(WHITE);

    Original_Test_Loop();

    gfx->setTextSize(1);
    gfx->fillScreen(MAROON);
}

void loop()
{
    if (millis() > CycleTime)
    {
        GFX_Print_Time_Info_Loop();
        CycleTime = millis() + 1000;
    }

    if (CST226SE->IIC_Interrupt_Flag == true)
    {
        CST226SE->IIC_Interrupt_Flag = false;

        uint8_t fingers_number = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

        if (fingers_number > 0)
        {
            Vibration_Start();

            int32_t touch_x = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            int32_t touch_y = CST226SE->IIC_Read_Device_Value(CST226SE->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);

            switch (Image_Flag)
            {
            case 0:
                gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)gImage_1, LCD_WIDTH, LCD_HEIGHT); // RGB
                break;
            case 1:
                gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)gImage_2, LCD_WIDTH, LCD_HEIGHT); // RGB
                break;

            default:
                break;
            }

            Image_Flag++;

            if (Image_Flag > 1)
            {
                Image_Flag = 0;
            }

            Serial.printf("[1] point x: %d  point y: %d \r\n", touch_x, touch_y);

            gfx->setCursor(touch_x, touch_y);
            gfx->setTextColor(RED);
            gfx->printf("[1] point x: %d  point y: %d \r\n", touch_x, touch_y);
        }
    }
}

// optional
void audio_info(const char *info)
{
    Serial.print("info        ");
    Serial.println(info);
}
void audio_id3data(const char *info)
{ // id3 metadata
    Serial.print("id3data     ");
    Serial.println(info);
}
void audio_eof_mp3(const char *info)
{ // end of file
    Serial.print("eof_mp3     ");
    Serial.println(info);
}
void audio_showstation(const char *info)
{
    Serial.print("station     ");
    Serial.println(info);
}
void audio_showstreamtitle(const char *info)
{
    Serial.print("streamtitle ");
    Serial.println(info);
}
void audio_bitrate(const char *info)
{
    Serial.print("bitrate     ");
    Serial.println(info);
}
void audio_commercial(const char *info)
{ // duration in sec
    Serial.print("commercial  ");
    Serial.println(info);
}
void audio_icyurl(const char *info)
{ // homepage
    Serial.print("icyurl      ");
    Serial.println(info);
}
void audio_lasthost(const char *info)
{ // stream URL played
    Serial.print("lasthost    ");
    Serial.println(info);
}
