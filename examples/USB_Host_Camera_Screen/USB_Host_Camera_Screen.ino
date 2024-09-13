/*
 * @Description: 
            USB Host Device, Example Program to Display USB Camera Data on the Screen
 * @Author: LILYGO_L
 * @Date: 2024-04-12 13:15:30
 * @LastEditTime: 2024-09-13 21:16:17
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include "USB_STREAM.h"
#include <Arduino_GFX_Library.h>
#include "pin_config.h"
#include "JPEGDEC.h"
#include "Arduino_DriveBus_Library.h"

#define OTG_SWITCH 0

static size_t CycleTime = 0;

static bool OTG_Flag = 0;

JPEGDEC jpeg;

Arduino_DataBus *bus = new Arduino_HWSPI(
    LCD_DC /* DC */, LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_MOSI /* MOSI */, LCD_MISO /* MISO */);

Arduino_GFX *gfx = new Arduino_ST7796(
    bus, LCD_RST /* RST */, 1 /* rotation */, true /* IPS */,
    LCD_WIDTH /* width */, LCD_HEIGHT /* height */,
    0 /* col offset 1 */, 0 /* row offset 1 */, 49 /* col_offset2 */, 0 /* row_offset2 */);

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
    std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);

std::unique_ptr<Arduino_IIC> SY6970(new Arduino_SY6970(IIC_Bus, SY6970_DEVICE_ADDRESS,
                                                       DRIVEBUS_DEFAULT_VALUE, DRIVEBUS_DEFAULT_VALUE));

// pixel drawing callback
static int drawMCU(JPEGDRAW *pDraw)
{
    // Serial.printf("Draw pos = (%d, %d), size = %d x %d\n", pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);

    gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
    return 1;
} /* drawMCU() */

/* Define the camera frame callback function implementation */
static void onCameraFrameCallback(uvc_frame *frame, void *user_ptr)
{
    // Serial.printf("uvc callback! frame_format = %d, seq = %" PRIu32 ", width = %" PRIu32", height = %" PRIu32 ", length = %u, ptr = %d\n",
    //          frame->frame_format, frame->sequence, frame->width, frame->height, frame->data_bytes, (int)user_ptr);

    if (jpeg.openRAM((uint8_t *)frame->data, frame->data_bytes, drawMCU))
    {
        if (millis() > CycleTime)
        {
            Serial.println("Successfully opened JPEG image");
            Serial.printf("Image size: %d x %d, orientation: %d, bpp: %d\n", jpeg.getWidth(),
                          jpeg.getHeight(), jpeg.getOrientation(), jpeg.getBpp());

            CycleTime = millis() + 1000;
        }

        if (jpeg.hasThumb())
        {
            Serial.printf("Thumbnail present: %d x %d\n", jpeg.getThumbWidth(), jpeg.getThumbHeight());
        }

        jpeg.setMaxOutputSize(MAX_BUFFERED_PIXELS / 16 / 16);
        jpeg.setPixelType(RGB565_BIG_ENDIAN); // The SPI LCD wants the 16-bit pixels in big-endian order

        // Draw the thumbnail image in the middle of the display (upper left corner = 120,100) at 1/4 scale
        if (jpeg.decode(80, 21, JPEG_SCALE_QUARTER))
        {
            // Serial.println("Image decoding successful");
        }
        jpeg.close();
        vTaskDelay(1); // 看门狗喂狗
    }

    // gfx->draw16bitBeRGBBitmap(0, 0, (uint16_t *)frame->data, 200, 200);
}

void setup()
{
    // Serial.begin(115200);

    pinMode(LCD_BL, OUTPUT);
    digitalWrite(LCD_BL, HIGH);

    pinMode(OTG_SWITCH, INPUT_PULLUP);

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
    // 输入电流限制设置为600mA
    SY6970->IIC_Write_Device_Value(SY6970->Arduino_IIC_Power::Device_Value::POWER_DEVICE_INPUT_CURRENT_LIMIT, 600);
    // 快速充电电流限制设置为2112mA
    SY6970->IIC_Write_Device_Value(SY6970->Arduino_IIC_Power::Device_Value::POWER_DEVICE_FAST_CHARGING_CURRENT_LIMIT, 2112);
    // 预充电电流限制设置为192mA
    SY6970->IIC_Write_Device_Value(SY6970->Arduino_IIC_Power::Device_Value::POWER_DEVICE_PRECHARGE_CHARGING_CURRENT_LIMIT, 192);
    // 终端充电电流限制设置为320mA
    SY6970->IIC_Write_Device_Value(SY6970->Arduino_IIC_Power::Device_Value::POWER_DEVICE_TERMINATION_CHARGING_CURRENT_LIMIT, 320);
    // OTG电流限制设置为500mA
    SY6970->IIC_Write_Device_Value(SY6970->Arduino_IIC_Power::Device_Value::POWER_DEVICE_OTG_CHARGING_LIMIT, 500);

    gfx->begin();
    gfx->fillScreen(WHITE);

    // Instantiate an object
    USB_STREAM *usb = new USB_STREAM();

    // allocate memory
    uint8_t *_xferBufferA = (uint8_t *)malloc(55 * 1024);
    assert(_xferBufferA != NULL);
    uint8_t *_xferBufferB = (uint8_t *)malloc(55 * 1024);
    assert(_xferBufferB != NULL);
    uint8_t *_frameBuffer = (uint8_t *)malloc(55 * 1024);
    assert(_frameBuffer != NULL);

    // Config the parameter
    usb->uvcConfiguration(640, 480, FRAME_INTERVAL_FPS_30, 55 * 1024, _xferBufferA, _xferBufferB, 55 * 1024, _frameBuffer);

    // Register the camera frame callback function
    usb->uvcCamRegisterCb(&onCameraFrameCallback, NULL);

    usb->start();

    usb->connectWait(1000);
    delay(5000);

    usb->uvcCamSuspend(NULL);
    delay(5000);

    usb->uvcCamResume(NULL);

    /*Dont forget to free the allocated memory*/
    // free(_xferBufferA);
    // free(_xferBufferB);
    // free(_frameBuffer);
}

void loop()
{
    if (digitalRead(OTG_SWITCH) == LOW)
    {
        delay(300);

        OTG_Flag = !OTG_Flag;
        if (OTG_Flag == 0)
        {
            SY6970->IIC_Write_Device_State(SY6970->Arduino_IIC_Power::Device::POWER_DEVICE_OTG_MODE,
                                           SY6970->Arduino_IIC_Power::Device_State::POWER_DEVICE_OFF);
            SY6970->IIC_Write_Device_State(SY6970->Arduino_IIC_Power::Device::POWER_DEVICE_CHARGING_MODE,
                                           SY6970->Arduino_IIC_Power::Device_State::POWER_DEVICE_ON);
        }
        else
        {
            SY6970->IIC_Write_Device_State(SY6970->Arduino_IIC_Power::Device::POWER_DEVICE_CHARGING_MODE,
                                           SY6970->Arduino_IIC_Power::Device_State::POWER_DEVICE_OFF);
            SY6970->IIC_Write_Device_State(SY6970->Arduino_IIC_Power::Device::POWER_DEVICE_OTG_MODE,
                                           SY6970->Arduino_IIC_Power::Device_State::POWER_DEVICE_ON);
        }
    }
    vTaskDelay(1);
}
