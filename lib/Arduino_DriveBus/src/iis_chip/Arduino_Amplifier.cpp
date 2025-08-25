/*
 * @Description: Arduino_Amplifier.cpp
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-12-21 14:04:34
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2024-12-12 11:01:41
 * @License: GPL 3.0
 */

#include "Arduino_Amplifier.h"

Arduino_Amplifier::Arduino_Amplifier(std::shared_ptr<Arduino_IIS_DriveBus> bus)
    : Arduino_IIS(bus)
{
}

bool Arduino_Amplifier::begin(i2s_mode_t iis_mode, ad_iis_data_mode_t device_state, i2s_channel_fmt_t channel_mode,
                              int8_t bits_per_sample, int32_t sample_rate)
{
    return Arduino_IIS::begin(iis_mode, device_state, channel_mode, bits_per_sample, sample_rate);
}

bool Arduino_Amplifier::IIS_Write_Data(const void *data, size_t bytes)
{
    return _bus->IIS_Write(data, bytes);
}

bool Arduino_Amplifier::end()
{
    return Arduino_IIS::end();
}