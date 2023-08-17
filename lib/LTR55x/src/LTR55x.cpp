#include "LTR55x.h"
#include <Arduino.h>

/* LTR5XX Registers address*/
#define LTR5XX_ALS_CONTR_REG 0x80
#define LTR5XX_PS_CONTR_REG 0x81
#define LTR5XX_PS_LED_REG 0x82
#define LTR5XX_PS_N_PULSES_REG 0x83
#define LTR5XX_PS_MEAS_RATE_REG 0x84
#define LTR5XX_ALS_MEAS_PATE_REG 0x85

#ifdef LTR507
#define LTR5XX_ALS_DATA_0_REG 0x88
#define LTR5XX_ALS_DATA_1_REG 0x89
#define LTR5XX_ALS_PS_STATUS_REG 0x8A
#define LTR5XX_PS_DATA_LOW_REG 0x8B
#define LTR5XX_PS_DATA_HIGH_REG 0x8C
#define LTR5XX_ALS_DATA_CH1_0_REG 0x8D
#define LTR5XX_ALS_DATA_CH1_1_REG 0x8E
#define LTR5XX_ALS_DATA_CH1_2_REG 0x8F
#define LTR5XX_ALS_DATA_CH2_0_REG 0x90
#define LTR5XX_ALS_DATA_CH2_1_REG 0x91
#define LTR5XX_ALS_DATA_CH2_2_REG 0x92
#define LTR5XX_INTERRUPT_REG 0x98
#define LTR5XX_PS_THRES_UP_0_REG 0x99
#define LTR5XX_PS_THRES_UP_1_REG 0x9A
#define LTR5XX_PS_THRES_LOW_0_REG 0x9B
#define LTR5XX_PS_THRES_LOW_1_REG 0x9C
#define LTR5XX_ALS_THRES_UP_0_REG 0x9E
#define LTR5XX_ALS_THRES_UP_1_REG 0x9F
#define LTR5XX_ALS_THRES_LOW_0_REG 0xA0
#define LTR5XX_ALS_THRES_LOW_1_REG 0xA1
#define LTR5XX_INTERRUPT_PERSIST_REG 0xA4
#endif

#ifdef LTR55X
#define LTR5XX_ALS_PS_STATUS_REG 0x8C
#define LTR5XX_PS_DATA_LOW_REG 0x8D
#define LTR5XX_PS_DATA_HIGH_REG 0x8E
#define LTR5XX_ALS_DATA_CH1_0_REG 0x88
#define LTR5XX_ALS_DATA_CH1_1_REG 0x89
#define LTR5XX_ALS_DATA_CH0_0_REG 0x8A
#define LTR5XX_ALS_DATA_CH0_1_REG 0x8B
#define LTR5XX_INTERRUPT_REG 0x8F
#define LTR5XX_PS_THRES_UP_0_REG 0x90
#define LTR5XX_PS_THRES_UP_1_REG 0x91
#define LTR5XX_PS_THRES_LOW_0_REG 0x92
#define LTR5XX_PS_THRES_LOW_1_REG 0x93
#define LTR5XX_ALS_THRES_UP_0_REG 0x97
#define LTR5XX_ALS_THRES_UP_1_REG 0x98
#define LTR5XX_ALS_THRES_LOW_0_REG 0x99
#define LTR5XX_ALS_THRES_LOW_1_REG 0x9A
#define LTR5XX_INTERRUPT_PERSIST_REG 0x9E
#endif
/*===========================================================================*/

/*===========================================================================*/
/*LTR55X Registers operating parameters*/
#ifdef LTR55X
#define LTR5XX_ALS_MODE_MASK 0x01
#define LTR5XX_ALS_MODE_SHIFT 0

#define LTR5XX_ALS_GAIN_MASK 0x1C
#define LTR5XX_ALS_GAIN_SHIFT 2

#define LTR5XX_ALS_INTEGRATION_TIME_MASK 0x38
#define LTR5XX_ALS_INTEGRATION_TIME_SHIFT 3

#define LTR5XX_ALS_MEASURE_RATE_MASK 0x07
#define LTR5XX_ALS_MEASURE_RATE_SHIFT 0

#define LTR5XX_ALS_DATA_LOW_MASK 0xFF
#define LTR5XX_ALS_DATA_HIGH_MASK 0xFF

#define LTR5XX_PS_PULSES_MASK 0x0F

#define LTR5XX_PS_MEAS_RATE_MASK 0x0F

#define LTR5XX_VALID_PS_DATA_MASK 0x80
#define LTR5XX_VALID_PS_DATA_SHIFT 7
#endif

/*LTR507 Registers operating parameters*/
#ifdef LTR507
#define LTR5XX_ALS_MODE_MASK 0x02
#define LTR5XX_ALS_MODE_SHIFT 0

#define LTR5XX_ALS_GAIN_MASK 0x18
#define LTR5XX_ALS_GAIN_SHIFT 3

#define LTR5XX_ALS_ADC_BIT_WIDTH_MASK 0xE0
#define LTR5XX_ALS_ADC_BIT_WIDTH_SHIFT 5

#define LTR5XX_ALS_MEASURE_RATE_MASK 0x07
#define LTR5XX_ALS_MEASURE_RATE_SHIFT 0

#define LTR5XX_ALS_DATA_LOW_MASK 0xF0
#define LTR5XX_ALS_DATA_MID_MASK 0xFF
#define LTR5XX_ALS_DATA_HIGH_MASK 0xFF

#define LTR5XX_PS_PULSES_MASK 0xFF

#define LTR5XX_PS_MEAS_RATE_MASK 0x07

#define LTR5XX_VALID_PS_DATA_MASK 0x10
#define LTR5XX_VALID_PS_DATA_SHIFT 4
#endif

/*LTR55X and LTR507 common Registers operating parameters*/
#define LTR5XX_PS_MODE_MASK 0x02
#define LTR5XX_PS_MODE_SHIFT 1

#define LTR5XX_LED_PULSE_FREQ_MASK 0xE0
#define LTR5XX_LED_PULSE_FREQ_SHIFT 5

#define LTR5XX_LED_DUTY_CYCLE_MASK 0x18
#define LTR5XX_LED_DUTY_CYCLE_SHIFT 3

#define LTR5XX_LED_PEAK_CURRENT_MASK 0x07

#define LTR5XX_PS_DATA_LOW_MASK 0xFF
#define LTR5XX_PS_DATA_HIGH_MASK 0x07

#define LTR5XX_ALS_INTERRUPT_STATUS_MASK 0x08
#define LTR5XX_ALS_INTERRUPT_STATUS_SHIFT 3

#define LTR5XX_PS_INTERRUPT_STATUS_MASK 0x02
#define LTR5XX_PS_INTERRUPT_STATUS_SHIFT 1

#define LTR5XX_INTERRUPT_POLARITY_MASK 0x04
#define LTR5XX_INTERRUPT_POLARITY_SHIFT 2

#define LTR5XX_INTERRUPT_MODE_MASK 0x03

#define LTR5XX_PS_THRES_UP_1_MASK 0x07
#define LTR5XX_PS_THRES_LOW_1_MASK 0x07

#define LTR5XX_PS_INTERRUPT_PERSIST_MASK 0xF0
#define LTR5XX_PS_INTERRUPT_PERSIST_SHIFT 4

#define LTR5XX_ALS_INTERRUPT_PERSIST_MASK 0x0F

LTR55x::LTR55x(/* args */)
{
}

LTR55x::~LTR55x()
{
}

bool LTR55x::begin()
{
    Wire.beginTransmission(LTR55X_ADDR);
    byte error = Wire.endTransmission();

    if (error != 0)
        return false;
    // 数据清零
    set_als_mode(0x02);
    // 配置置工作数据
    set_als_mode(0x0d); // 设置8X增益
    set_ps_mode(0x1b);
    /*set_led_pulse(LTR5XX_LED_PULSE_FREQ_60KHZ);
    set_led_current(LTR5XX_LED_PEAK_CURRENT_100MA);
    set_als_gain(LTR5XX_ALS_MEASUREMENT_RATE_500MS);*/
    set_led_pulse(LTR5XX_LED_PULSE_FREQ_40KHZ);
    set_led_current(LTR5XX_LED_PEAK_CURRENT_50MA);
    set_als_gain(LTR5XX_ALS_GAIN_96X);

    set_interrupt_status(0x00); //默认状态不启用中断
    return true;
}
bool LTR55x::sleep()
{
    begin();
    set_ps_mode(0x00);
    return true;
}
void LTR55x::read(uint8_t reg_addr, uint8_t *buffer, uint8_t size)
{
    Wire.beginTransmission(LTR55X_ADDR);
    Wire.write(reg_addr);
    Wire.endTransmission();

    Wire.requestFrom(LTR55X_ADDR, size);
    uint8_t i = 0;
    while (Wire.available() > 0 && i < size)
    {
        *(buffer + i) = Wire.read();
        i++;
    }
}
void LTR55x::write(uint8_t reg_addr, uint8_t *buffer, uint8_t size)
{
    Wire.beginTransmission(LTR55X_ADDR);
    Wire.write(reg_addr);
    for (uint8_t i = 0; i < size; i++)
    {
        Wire.write(*(buffer + i));
    }

    Wire.endTransmission();
}
// 设置ALS工作模式
void LTR55x::set_als_mode(uint8_t mode)
{
    uint8_t config;
    read(LTR5XX_ALS_CONTR_REG, &config, 1);
    // 将软件重置及激活状态清零,方便后面设置新值
    // 清零位(为1清零) 0000 0011
    config &= (~0x03);
    config |= mode; // 设置状态值
    write(LTR5XX_ALS_CONTR_REG, &config, 1);
}
// 设置PS工作模式
void LTR55x::set_ps_mode(uint8_t mode)
{
    uint8_t config;
    read(LTR5XX_PS_CONTR_REG, &config, 1);
    // 将激活状态清零,方便后面设置新值
    // 清零位(为1清零) 0000 0011
    config &= (~0x03);
    config |= mode; // 设置状态值
    write(LTR5XX_PS_CONTR_REG, &config, 1);
}
// 设置ALS增益
void LTR55x::set_als_gain(uint8_t mode)
{
    uint8_t config;
    read(LTR5XX_ALS_CONTR_REG, &config, 1);
    // 将状态清零,方便后面设置新值
    // 清零位(为1清零) 0001 1100
    config &= (~0x1c);
    config |= mode << 2; // 设置状态值
    write(LTR5XX_ALS_CONTR_REG, &config, 1);
}
// 设置PS增益
void LTR55x::set_ps_gain(uint8_t mode)
{
    uint8_t config;
    read(LTR5XX_PS_CONTR_REG, &config, 1);
    // 将状态清零,方便后面设置新值
    // 清零位(为1清零) 0000 1100
    config &= ~(0x03 << 2);
    config |= mode << 2; // 设置状态值
    write(LTR5XX_PS_CONTR_REG, &config, 1);
}
// 设置LED工作频率
void LTR55x::set_led_pulse(uint8_t mode)
{
    uint8_t config;
    read(LTR5XX_PS_LED_REG, &config, 1);
    // 将状态清零,方便后面设置新值
    // 清零位(为1清零) 1111 1000
    config &= ~(0x1f << 3);
    config |= mode << 5; // 设置状态值
    config |= 0x01 << 3; // 设置设置占空比 50% (LTR-507仅支持50%)
    write(LTR5XX_PS_LED_REG, &config, 1);
}
// 设置LED工作电流
void LTR55x::set_led_current(uint8_t mode)
{
    uint8_t config;
    read(LTR5XX_PS_LED_REG, &config, 1);
    // 将状态清零,方便后面设置新值
    // 清零位(为1清零) 0001 1111
    config &= ~(0x07);
    config |= mode;      // 设置状态值
    config |= 0x01 << 3; // 设置设置占空比 50% (LTR-507仅支持50%)
    write(LTR5XX_PS_LED_REG, &config, 1);
}
// 设置PS测量速率
void LTR55x::set_ps_meas_rate(uint8_t mode)
{
    uint8_t config = 0x00;
    config |= mode; // 设置状态值
    write(LTR5XX_PS_MEAS_RATE_REG, &config, 1);
}
// 设置ALS测量速率
void LTR55x::set_als_meas_rate(uint8_t mode)
{
    uint8_t config;
    read(LTR5XX_ALS_MEAS_PATE_REG, &config, 1);
    // 将状态清零,方便后面设置新值
    // 清零位(为1清零) 0000 0111
    config &= ~(0x07);
    config |= mode; // 设置状态值
    write(LTR5XX_ALS_MEAS_PATE_REG, &config, 1);
}

// 设置中断状态
void LTR55x::set_interrupt_status(uint8_t mode)
{
    uint8_t config;
    read(LTR5XX_INTERRUPT_REG, &config, 1);
    // 将状态清零,方便后面设置新值
    // 清零位(为1清零) 0000 0011
    config &= ~(0x03);
    config |= mode; // 设置状态值
    write(LTR5XX_INTERRUPT_REG, &config, 1);
}
// 设置中断极性 即中断时INT引脚输出高还是低电平 默认为低
void LTR55x::set_interrupt_polarity(uint8_t polarity)
{
    uint8_t config;
    read(LTR5XX_INTERRUPT_REG, &config, 1);
    // 将状态清零,方便后面设置新值
    // 清零位(为1清零) 0000 0100
    config &= ~(0x01 << 2);
    config |= polarity; // 设置状态值
    write(LTR5XX_INTERRUPT_REG, &config, 1);
}
// 设置PS中断阀值
void LTR55x::set_ps_interrupt_threshold(uint16_t upper_threshold, uint16_t low_threshold)
{
    uint8_t buffer[4] = {0};
    buffer[0] = upper_threshold & 0xFF;
    buffer[1] = (upper_threshold >> 8) & LTR5XX_PS_THRES_UP_1_MASK;
    buffer[2] = low_threshold & 0xFF;
    buffer[3] = (low_threshold >> 8) & LTR5XX_PS_THRES_LOW_1_MASK;

    write(LTR5XX_PS_THRES_UP_0_REG, &buffer[0], 4);
}
// 设置ALS中断阀值
void LTR55x::set_als_interrupt_threshold(uint16_t upper_threshold, uint16_t low_threshold)
{
    uint8_t buffer[4] = {0};
    buffer[0] = upper_threshold & 0xFF;
    buffer[1] = (upper_threshold >> 8) & 0xFF;
    buffer[2] = low_threshold & 0xFF;
    buffer[3] = (low_threshold >> 8) & 0xFF;

    write(LTR5XX_ALS_THRES_UP_0_REG, &buffer[0], 4);
}
// 设置测量值达到多少次后触发中断
void LTR55x::set_trigger_interrupt_number(uint8_t ps_value, uint8_t als_vaule)
{
    uint8_t config = 0;

    config = (ps_value << 4) & 0xF0;
    config |= als_vaule & 0x0F;

    write(LTR5XX_INTERRUPT_PERSIST_REG, &config, 1);
}

uint16_t LTR55x::get_als_value()
{
    uint8_t buffer[4] = {0};
    uint16_t result;

    read(LTR5XX_ALS_DATA_CH1_0_REG, &buffer[0], 4);

    uint16_t ch1_value, ch0_value;
    ch1_value = (buffer[1] << 8) | buffer[0];
    ch0_value = (buffer[3] << 8) | buffer[2];
    result = (ch1_value + ch0_value) >> 1;
    return result;
}
uint16_t LTR55x::get_ps_value()
{
    uint8_t buffer[2];
    uint16_t result;
    read(LTR5XX_PS_DATA_LOW_REG, &buffer[0], 2);
    buffer[0] &= LTR5XX_PS_DATA_LOW_MASK;
    buffer[1] &= LTR5XX_PS_DATA_HIGH_MASK;
    result = (buffer[1] << 8) | buffer[0];
    return result;
}
uint8_t LTR55x::get_als_interrupt_status()
{
    uint8_t read_reg, int_status;
    read(LTR5XX_ALS_PS_STATUS_REG, &read_reg, 1);
    int_status = (read_reg & LTR5XX_ALS_INTERRUPT_STATUS_MASK) >> LTR5XX_ALS_INTERRUPT_STATUS_SHIFT;
    return int_status;
}
uint8_t LTR55x::get_ps_interrupt_status()
{
    uint8_t read_reg, int_status;
    read(LTR5XX_ALS_PS_STATUS_REG, &read_reg, 1);
    int_status = (read_reg & LTR5XX_PS_INTERRUPT_STATUS_MASK) >> LTR5XX_PS_INTERRUPT_STATUS_SHIFT;
    return int_status;
}