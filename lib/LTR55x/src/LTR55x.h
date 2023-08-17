#ifndef YUTOO_HEADER_LTR55x__
#define YUTOO_HEADER_LTR55x__

#include <stdint.h>
#include <Wire.h>

#ifndef LTR55X
#define LTR55X
#endif

/*if use LTR507*/
//#ifndef LTR507
//  #define LTR507
//#endif

/*LTR-55x address is can't change*/
#define LTR55X_ADDR 0x23
/*LTR-507 SEL pin is "Float"*/
#define LTR507_SEL_FLOAT_ADDR 0x23
/*LTR-507 SEL pin is "VDD"*/
#define LTR507_SEL_VDD_ADDR 0x3B
/*LTR-507 SEL pin is "GND"*/
#define LTR507_SEL_GND_ADDR 0x3A

/*===========================================================================*/
/*LTR55X Registers operating parameters*/
#ifdef LTR55X
#define LTR5XX_ALS_STAND_BY_MODE 0
#define LTR5XX_ALS_ACTIVE_MODE 1

#define LTR5XX_ALS_GAIN_1X 0x00 // default
#define LTR5XX_ALS_GAIN_2X 0x01
#define LTR5XX_ALS_GAIN_4X 0x02
#define LTR5XX_ALS_GAIN_8X 0x03
#define LTR5XX_ALS_GAIN_48X 0x06
#define LTR5XX_ALS_GAIN_96X 0x07

// als measurement time must be set to be equal or larger than integration time
#define LTR5XX_ALS_INTEGRATION_TIME_50MS 0x01
#define LTR5XX_ALS_INTEGRATION_TIME_100MS 0x00 // default
#define LTR5XX_ALS_INTEGRATION_TIME_150MS 0x04
#define LTR5XX_ALS_INTEGRATION_TIME_200MS 0x02
#define LTR5XX_ALS_INTEGRATION_TIME_250MS 0x05
#define LTR5XX_ALS_INTEGRATION_TIME_300MS 0x06
#define LTR5XX_ALS_INTEGRATION_TIME_350MS 0x07
#define LTR5XX_ALS_INTEGRATION_TIME_400MS 0x03

#define LTR5XX_ALS_MEASUREMENT_RATE_50MS 0x00
#define LTR5XX_ALS_MEASUREMENT_RATE_100MS 0x01
#define LTR5XX_ALS_MEASUREMENT_RATE_200MS 0x02
#define LTR5XX_ALS_MEASUREMENT_RATE_500MS 0x03 // default
#define LTR5XX_ALS_MEASUREMENT_RATE_1000MS 0x04
#define LTR5XX_ALS_MEASUREMENT_RATE_2000MS 0x05

#define LTR5XX_LED_CURRENT_DUTY_PER25 0x00
#define LTR5XX_LED_CURRENT_DUTY_PER50 0x01
#define LTR5XX_LED_CURRENT_DUTY_PER75 0x02
#define LTR5XX_LED_CURRENT_DUTY_PER100 0x03 // default

#define LTR5XX_PS_MEASUREMENT_RATE_10MS 0x08
#define LTR5XX_PS_MEASUREMENT_RATE_50MS 0x00
#define LTR5XX_PS_MEASUREMENT_RATE_70MS 0x01
#define LTR5XX_PS_MEASUREMENT_RATE_100MS 0x02 // default
#define LTR5XX_PS_MEASUREMENT_RATE_200MS 0x03
#define LTR5XX_PS_MEASUREMENT_RATE_500MS 0x04
#define LTR5XX_PS_MEASUREMENT_RATE_1000MS 0x05
#define LTR5XX_PS_MEASUREMENT_RATE_2000MS 0x06
#endif

/*LTR507 Registers operating parameters*/
#ifdef LTR507
#define LTR5XX_ALS_STAND_BY_MODE 0
#define LTR5XX_ALS_ACTIVE_MODE 2

#define LTR5XX_ALS_GAIN_RANGE1 0x00 // default
#define LTR5XX_ALS_GAIN_RANGE2 0x01
#define LTR5XX_ALS_GAIN_RANGE3 0x02
#define LTR5XX_ALS_GAIN_RANGE4 0x03

#define LTR5XX_ALS_ADC_BIT_WIDTH_20BIT 0x00 // integration time = 1200ms
#define LTR5XX_ALS_ADC_BIT_WIDTH_19BIT 0x01 // integration time = 600ms
#define LTR5XX_ALS_ADC_BIT_WIDTH_18BIT 0x02 // integration time = 300ms
#define LTR5XX_ALS_ADC_BIT_WIDTH_17BIT 0x03 // integration time = 150ms
#define LTR5XX_ALS_ADC_BIT_WIDTH_16BIT 0x04 // integration time = 75ms default
#define LTR5XX_ALS_ADC_BIT_WIDTH_12BIT 0x05 // integration time = 4.685ms
#define LTR5XX_ALS_ADC_BIT_WIDTH_8BIT 0x06  // integration time = 292us
#define LTR5XX_ALS_ADC_BIT_WIDTH_4BIT 0x07  // integration time = 18us

// als measurement time must be set to be equal or larger than integration time
#define LTR5XX_ALS_MEASUREMENT_RATE_100MS 0x00
#define LTR5XX_ALS_MEASUREMENT_RATE_200MS 0x01
#define LTR5XX_ALS_MEASUREMENT_RATE_500MS 0x02 // default
#define LTR5XX_ALS_MEASUREMENT_RATE_1000MS 0x03
#define LTR5XX_ALS_MEASUREMENT_RATE_2000MS 0x04

#define LTR5XX_LED_CURRENT_DUTY_DEFAULT 0x01 // LTR507 led duty cycle must write as 01

#define LTR5XX_PS_MEASUREMENT_RATE_12_5MS 0x00
#define LTR5XX_PS_MEASUREMENT_RATE_50MS 0x01
#define LTR5XX_PS_MEASUREMENT_RATE_70MS 0x02
#define LTR5XX_PS_MEASUREMENT_RATE_100MS 0x03 // default
#define LTR5XX_PS_MEASUREMENT_RATE_200MS 0x04
#define LTR5XX_PS_MEASUREMENT_RATE_500MS 0x05
#define LTR5XX_PS_MEASUREMENT_RATE_1000MS 0x06
#define LTR5XX_PS_MEASUREMENT_RATE_2000MS 0x07
#endif

/*LTR55X and LTR507 common Registers operating parameters*/
#define LTR5XX_PS_STAND_BY_MODE 0
#define LTR5XX_PS_ACTIVE_MODE 1

#define LTR5XX_LED_PULSE_FREQ_30KHZ 0x00
#define LTR5XX_LED_PULSE_FREQ_40KHZ 0x01
#define LTR5XX_LED_PULSE_FREQ_50KHZ 0x02
#define LTR5XX_LED_PULSE_FREQ_60KHZ 0x03 // default
#define LTR5XX_LED_PULSE_FREQ_70KHZ 0x04
#define LTR5XX_LED_PULSE_FREQ_80KHZ 0x05
#define LTR5XX_LED_PULSE_FREQ_90KHZ 0x06
#define LTR5XX_LED_PULSE_FREQ_100KHZ 0x07

#define LTR5XX_LED_PEAK_CURRENT_5MA 0x00
#define LTR5XX_LED_PEAK_CURRENT_10MA 0x01
#define LTR5XX_LED_PEAK_CURRENT_20MA 0x02
#define LTR5XX_LED_PEAK_CURRENT_50MA 0x03  // LTR507 default
#define LTR5XX_LED_PEAK_CURRENT_100MA 0x04 // LTR55X default

#define LTR5XX_INTERRUPT_TRIGGER 1

#define LTR5XX_INTERRUPT_POLARITY_LOGIC_0 0 // interrupt tigger INT PIN is low level(INT PIN must Connect pull-up resistor to VDD) default
#define LTR5XX_INTERRUPT_POLARITY_LOGIC_1 1 // interrupt tigger INT PIN is high level(INT PIN must Connect pull-down resistor to GND)

#define LTR5XX_INTERRUPT_MODE_INACTIVE 0       // interrupt pin is inactive/high impedance state (default)
#define LTR5XX_INTERRUPT_MODE_PS_TRIGGER 1     // only ps can tigger interrupt
#define LTR5XX_INTERRUPT_MODE_ALS_TRIGGER 2    // only als can tigger interrupt
#define LTR5XX_INTERRUPT_MODE_PS_ALS_TRIGGER 3 // both ps and als can tigger interrupt

#define LTR5XX_EVERY_VALUE_OUTSIDE_TIGGER_INTERRUPT 0 // every ps value outside threshold range
#define LTR5XX_2_VALUES_OUTSIDE_TIGGER_INTERRUPT 1
#define LTR5XX_3_VALUES_OUTSIDE_TIGGER_INTERRUPT 2
#define LTR5XX_4_VALUES_OUTSIDE_TIGGER_INTERRUPT 3
#define LTR5XX_5_VALUES_OUTSIDE_TIGGER_INTERRUPT 4
#define LTR5XX_6_VALUES_OUTSIDE_TIGGER_INTERRUPT 5
#define LTR5XX_7_VALUES_OUTSIDE_TIGGER_INTERRUPT 6
#define LTR5XX_8_VALUES_OUTSIDE_TIGGER_INTERRUPT 7
#define LTR5XX_9_VALUES_OUTSIDE_TIGGER_INTERRUPT 8
#define LTR5XX_10_VALUES_OUTSIDE_TIGGER_INTERRUPT 9
#define LTR5XX_11_VALUES_OUTSIDE_TIGGER_INTERRUPT 10
#define LTR5XX_12_VALUES_OUTSIDE_TIGGER_INTERRUPT 11
#define LTR5XX_13_VALUES_OUTSIDE_TIGGER_INTERRUPT 12
#define LTR5XX_14_VALUES_OUTSIDE_TIGGER_INTERRUPT 13
#define LTR5XX_15_VALUES_OUTSIDE_TIGGER_INTERRUPT 14
#define LTR5XX_16_VALUES_OUTSIDE_TIGGER_INTERRUPT 15
/*===========================================================================*/

class LTR55x
{
private:
    void read(uint8_t reg_addr, uint8_t *buffer, uint8_t size);
    void write(uint8_t reg_addr, uint8_t *buffer, uint8_t size);

public:
    LTR55x(/* args */);
    ~LTR55x();
    bool begin();
    bool sleep();

    void set_als_mode(uint8_t mode);
    void set_ps_mode(uint8_t mode);
    void set_als_gain(uint8_t mode);
    void set_ps_gain(uint8_t mode);
    void set_led_pulse(uint8_t mode);
    void set_led_current(uint8_t mode);
    void set_ps_meas_rate(uint8_t mode);
    void set_als_meas_rate(uint8_t mode);
    void set_interrupt_status(uint8_t mode);
    void set_interrupt_polarity(uint8_t polarity);
    void set_ps_interrupt_threshold(uint16_t upper_threshold, uint16_t low_threshold);
    void set_als_interrupt_threshold(uint16_t upper_threshold, uint16_t low_threshold);
    void set_trigger_interrupt_number(uint8_t ps_value, uint8_t als_vaule);

    uint16_t get_als_value();
    uint16_t get_ps_value();
    uint8_t get_als_interrupt_status();
    uint8_t get_ps_interrupt_status();
};

#endif