/*
 * @Description(CN):
 *      这是一个使用硬件IIS的IIS底层驱动文件
 *
 * @Description(EN):
 *      This is a bottom-layer IIS driver file using hardware IIS.
 *
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-12-20 15:45:17
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2024-12-12 11:01:34
 * @License: GPL 3.0
 */
#pragma once

#include "../Arduino_DriveBus.h"

class Arduino_HWIIS : public Arduino_IIS_DriveBus
{
public:
    Arduino_HWIIS(i2s_port_t iis_num = I2S_NUM_0, int8_t bclk = DRIVEBUS_DEFAULT_VALUE,
                  int8_t lrck = DRIVEBUS_DEFAULT_VALUE, int8_t data = DRIVEBUS_DEFAULT_VALUE);

    bool begin(i2s_mode_t iis_mode, ad_iis_data_mode_t device_state, i2s_channel_fmt_t channel_mode,
               int8_t bits_per_sample = DRIVEBUS_DEFAULT_VALUE, int32_t sample_rate = DRIVEBUS_DEFAULT_VALUE) override;

    size_t Read(void *data, size_t bytes) override;
    size_t Write(const void *data, size_t bytes) override;

    bool end() override;

private:
    i2s_port_t _iis_num;
    int8_t _bclk, _lrck, _data;
};