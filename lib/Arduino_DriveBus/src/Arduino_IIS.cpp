/*
 * @Description: Arduino_IIS.cpp
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-11-16 16:58:05
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2024-12-12 11:02:16
 * @License: GPL 3.0
 */
#include "Arduino_IIS.h"

Arduino_IIS::Arduino_IIS(std::shared_ptr<Arduino_IIS_DriveBus> bus)
    : _bus(bus)
{
}

bool Arduino_IIS::begin(i2s_mode_t iis_mode, ad_iis_data_mode_t device_state, i2s_channel_fmt_t channel_mode,
                        int8_t bits_per_sample, int32_t sample_rate)
{
    if (_bus->begin(iis_mode, device_state, channel_mode, bits_per_sample, sample_rate) == false)
    {
        log_e("->_bus->begin(device_state, sample_rate, bits_per_sample) fail");
        return false;
    }

    return true;
}

bool Arduino_IIS::IIS_Read_Data(void *data, size_t bytes)
{
    log_e("No 'IIS_Read_Data' fictional function has been created.");
    return -1;
}

bool Arduino_IIS::IIS_Write_Data(const void *data, size_t bytes)
{
    log_e("No 'IIS_Write_Data' fictional function has been created.");
    return -1;
}

bool Arduino_IIS::end()
{
    if (_bus->end() == false)
    {
        log_e("->_bus->end() fail");
        return false;
    }

    return true;
}
