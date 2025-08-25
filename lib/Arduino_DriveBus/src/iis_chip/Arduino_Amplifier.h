/*
 * @Description(CN): 功放扬声器、耳机DAC等需要IIS输出的芯片都兼容
 *
 * @Description(EN): Chipsets for amplified speakers, headphone DACs, etc.,
 *       requiring IIS output are compatible.
 *
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-12-21 14:23:07
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2024-12-12 10:33:26
 * @License: GPL 3.0
 */
#pragma once

#include "../Arduino_IIS.h"

class Arduino_Amplifier : public Arduino_IIS
{
public:
    Arduino_Amplifier(std::shared_ptr<Arduino_IIS_DriveBus> bus);

    bool begin(i2s_mode_t iis_mode, ad_iis_data_mode_t device_state, i2s_channel_fmt_t channel_mode,
               int8_t bits_per_sample = DRIVEBUS_DEFAULT_VALUE, int32_t sample_rate = DRIVEBUS_DEFAULT_VALUE) override;

    bool IIS_Write_Data(const void *data, size_t bytes) override;

    bool end() override;
};