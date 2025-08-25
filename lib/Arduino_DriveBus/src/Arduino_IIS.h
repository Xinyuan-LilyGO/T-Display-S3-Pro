/*
 * @Description(CN):
 *      Arduino_IIC用于管理IIC相关总线 他与文件夹xxx_chip里的底层驱动有关
 *
 * @Description(EN):
 *      Arduino_IIC is used for managing IIC related buses. It is related to the bottom-level
 *  drivers in the folder xxx_chip.
 *
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-11-16 16:58:05
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2024-12-12 11:02:13
 * @License: GPL 3.0
 */
#pragma once

#include "Arduino_DriveBus.h"

class Arduino_IIS
{
public:
    enum Device_Switch
    {
        CHANNEL_ON,
        CHANNEL_OFF,
    };

    Arduino_IIS(std::shared_ptr<Arduino_IIS_DriveBus> bus);

    virtual bool begin(i2s_mode_t iis_mode, ad_iis_data_mode_t device_state, i2s_channel_fmt_t channel_mode,
                       int8_t bits_per_sample = DRIVEBUS_DEFAULT_VALUE, int32_t sample_rate = DRIVEBUS_DEFAULT_VALUE);

    virtual bool IIS_Read_Data(void *data, size_t bytes);
    virtual bool IIS_Write_Data(const void *data, size_t bytes);

    virtual bool end();

protected:
    std::shared_ptr<Arduino_IIS_DriveBus> _bus;
};