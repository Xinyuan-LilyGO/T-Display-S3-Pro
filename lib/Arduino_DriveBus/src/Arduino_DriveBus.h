/*
 * @Description(CN):
 *      Arduino_DriveBus用于管理所有驱动总线 他与文件夹hardware里的底层驱动有关
 *
 *  @Description(EN):
 *      Arduino_DriveBus is used for managing all drive buses. It is related to the bottom-level
 *  drivers in the hardware folder.
 *
 * @version: V1.1.5
 * @Author: LILYGO_L
 * @Date: 2023-11-16 15:53:46
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2024-12-12 11:01:57
 * @License: GPL 3.0
 */
#pragma once

#include <Arduino.h>
#include <iostream>
#include <memory>
#include <vector>
#include <numeric>
#include <driver/i2s.h>

#define DRIVEBUS_DEFAULT_VALUE -1

enum Arduino_DriveBus_BufferOperation
{
    BO_BEGIN_TRANSMISSION,
    BO_WRITE,
    BO_WRITE_C8_D8,
    BO_END_TRANSMISSION,
    BO_DELAY,
};

enum ad_iis_data_mode_t
{
    AD_IIS_DATA_OUT,
    AD_IIS_DATA_IN,
};

class Arduino_IIC_DriveBus
{
public:
    Arduino_IIC_DriveBus();

    virtual bool begin(int32_t speed = DRIVEBUS_DEFAULT_VALUE) = 0;

    virtual void BeginTransmission(uint8_t device_address) = 0;
    virtual bool EndTransmission(void) = 0;
    virtual bool Write(uint8_t d) = 0;
    virtual bool Write(const uint8_t *data, size_t length) = 0;
    virtual uint8_t Read(void) = 0;
    virtual bool RequestFrom(uint8_t device_address, size_t length) = 0;
    virtual bool WriteC8D8(uint8_t c, uint8_t d);

    bool BufferOperation(uint8_t device_address, const uint8_t *operations, size_t length);

    bool IIC_Device_7Bit_Scan(std::vector<unsigned char> *device_address);

    bool IIC_Write_Data(uint8_t device_address, const uint8_t *data, size_t length);
    bool IIC_WriteC8_Data(uint8_t device_address, uint8_t c, const uint8_t *data, size_t length);
    bool IIC_WriteC16_Data(uint8_t device_address, uint16_t c, const uint8_t *data, size_t length);
    bool IIC_WriteC8D8(uint8_t device_address, uint8_t c, uint8_t d);
    bool IIC_WriteC16D8(uint8_t device_address, uint16_t c, uint8_t d);
    bool IIC_ReadC8_Data(uint8_t device_address, uint8_t c, uint8_t *d, size_t length);
    bool IIC_ReadC16_Data(uint8_t device_address, uint16_t c, uint8_t *d, size_t length);
    bool IIC_ReadC8_Delay_Data(uint8_t device_address, uint8_t c, uint32_t delay_ms, uint8_t *d, size_t length);
    bool IIC_ReadC8D8(uint8_t device_address, uint8_t c, uint8_t *d);
    bool IIC_ReadC16D8(uint8_t device_address, uint16_t c, uint8_t *d);

protected:
    int32_t _speed;
};

class Arduino_IIS_DriveBus
{
public:
    Arduino_IIS_DriveBus();

    virtual bool begin(i2s_mode_t iis_mode, ad_iis_data_mode_t device_state, i2s_channel_fmt_t channel_mode,
                       int8_t bits_per_sample = DRIVEBUS_DEFAULT_VALUE, int32_t sample_rate = DRIVEBUS_DEFAULT_VALUE) = 0;

    virtual size_t Read(void *data, size_t bytes) = 0;
    virtual size_t Write(const void *data, size_t bytes) = 0;

    bool IIS_Read(void *data, size_t bytes);
    bool IIS_Write(const void *data, size_t bytes);

    virtual bool end() = 0;

protected:
    i2s_mode_t _iis_mode;
    ad_iis_data_mode_t _device_state;
    i2s_channel_fmt_t _channel_mode;

    int32_t _sample_rate, _bits_per_sample;
};