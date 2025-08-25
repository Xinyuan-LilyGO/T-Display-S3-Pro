/*
 * @Description: Arduino_HWIIS.cpp
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-12-20 15:46:16
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2024-12-12 11:01:18
 * @License: GPL 3.0
 */
#include "Arduino_HWIIS.h"

Arduino_HWIIS::Arduino_HWIIS(i2s_port_t iis_num, int8_t bclk, int8_t lrck, int8_t data)
    : _iis_num(iis_num), _bclk(bclk), _lrck(lrck), _data(data)
{
}

bool Arduino_HWIIS::begin(i2s_mode_t iis_mode, ad_iis_data_mode_t device_state, i2s_channel_fmt_t channel_mode,
                          int8_t bits_per_sample, int32_t sample_rate)
{
    i2s_config_t iis_config;
    i2s_pin_config_t iis_pin_config;

    _iis_mode = iis_mode;
    _device_state = device_state;
    _channel_mode = channel_mode;
    _bits_per_sample = (bits_per_sample == DRIVEBUS_DEFAULT_VALUE) ? 16 : bits_per_sample;
    _sample_rate = (sample_rate == DRIVEBUS_DEFAULT_VALUE) ? 44100U : sample_rate;

    switch (_device_state)
    {
    case AD_IIS_DATA_OUT:
        iis_config = {
            .mode = (i2s_mode_t)(_iis_mode | I2S_MODE_TX),
            .sample_rate = (uint32_t)_sample_rate,
            .bits_per_sample = (i2s_bits_per_sample_t)_bits_per_sample,
            .channel_format = _channel_mode,
            .communication_format = I2S_COMM_FORMAT_STAND_I2S,
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
            .dma_buf_count = 8,
            .dma_buf_len = 1024,
            .use_apll = false,
            .tx_desc_auto_clear = true,
            .fixed_mclk = I2S_PIN_NO_CHANGE,
#if CONFIG_IDF_TARGET_ESP32C6
            .mclk_multiple = I2S_MCLK_MULTIPLE_512,
#endif
#if CONFIG_IDF_TARGET_ESP32S3
            .mclk_multiple = I2S_MCLK_MULTIPLE_DEFAULT,
#endif
            .bits_per_chan = I2S_BITS_PER_CHAN_16BIT,
#if SOC_I2S_SUPPORTS_TDM
            .chan_mask = I2S_CHANNEL_MONO,
            .total_chan = 0,
            .left_align = false,
            .big_edin = false,
            .bit_order_msb = false,
            .skip_msk = false,
#endif
        };

        iis_pin_config = {
            .mck_io_num = I2S_PIN_NO_CHANGE,
            .bck_io_num = _bclk,
            .ws_io_num = _lrck,
            .data_out_num = _data,
            .data_in_num = I2S_PIN_NO_CHANGE};
        break;

    case AD_IIS_DATA_IN:
        iis_config = {
            .mode = (i2s_mode_t)(_iis_mode | I2S_MODE_RX),
            .sample_rate = (uint32_t)_sample_rate,
            .bits_per_sample = (i2s_bits_per_sample_t)_bits_per_sample,
            .channel_format = _channel_mode,
            .communication_format = I2S_COMM_FORMAT_STAND_I2S,
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
            .dma_buf_count = 8,
            .dma_buf_len = 1024,
            .use_apll = false,
            .tx_desc_auto_clear = true,
            .fixed_mclk = I2S_PIN_NO_CHANGE,
#if CONFIG_IDF_TARGET_ESP32C6
            .mclk_multiple = I2S_MCLK_MULTIPLE_512,
#endif
#if CONFIG_IDF_TARGET_ESP32S3
            .mclk_multiple = I2S_MCLK_MULTIPLE_DEFAULT,
#endif
            .bits_per_chan = I2S_BITS_PER_CHAN_16BIT,
#if SOC_I2S_SUPPORTS_TDM
            .chan_mask = I2S_CHANNEL_MONO,
            .total_chan = 0,
            .left_align = false,
            .big_edin = false,
            .bit_order_msb = false,
            .skip_msk = false,
#endif
        };

        iis_pin_config = {
            .mck_io_num = I2S_PIN_NO_CHANGE,
            .bck_io_num = _bclk,
            .ws_io_num = _lrck,
            .data_out_num = I2S_PIN_NO_CHANGE,
            .data_in_num = _data};
        break;

    default:
        break;
    }

    if (i2s_driver_install(_iis_num, &iis_config, 0, NULL) != ESP_OK)
    {
        return false;
    }
    if (i2s_set_pin(_iis_num, &iis_pin_config) != ESP_OK)
    {
        return false;
    }
    return true;
}

size_t Arduino_HWIIS::Read(void *data, size_t bytes)
{
    size_t temp_buf;
    i2s_read(_iis_num, data, bytes, &temp_buf, portMAX_DELAY);
    return temp_buf;
}

size_t Arduino_HWIIS::Write(const void *data, size_t bytes)
{
    size_t temp_buf;
    i2s_write(_iis_num, data, bytes, &temp_buf, portMAX_DELAY);
    return temp_buf;
}

bool Arduino_HWIIS::end()
{
    if (i2s_driver_uninstall(_iis_num) != ESP_OK)
    {
        return false;
    }
    return true;
}
