/*
 * @Description(CN):
 *      Arduino_IIC用于管理IIC相关总线 他与文件夹xxx_chip里的底层驱动有关
 *
 * @Description(EN):
 *      Arduino_IIC is used for managing IIC related buses. It is related to the bottom-level
 *  drivers in the folder xxx_chip.
 *
 * @version: V1.1.5
 * @Author: LILYGO_L
 * @Date: 2023-11-16 16:58:05
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2025-01-03 10:21:50
 * @License: GPL 3.0
 */
#pragma once

#include "Arduino_DriveBus.h"
#include "Arduino_IIC_Chip.h"

class Arduino_IIC : public Arduino_IIC_Power, public Arduino_IIC_Touch, public Arduino_IIC_IMU, public Arduino_IIC_RTC, public Arduino_IIC_HAPTIC
{
public:
    Arduino_IIC(std::shared_ptr<Arduino_IIC_DriveBus> bus, uint8_t device_address,
                int8_t rst = DRIVEBUS_DEFAULT_VALUE, int8_t iqr = DRIVEBUS_DEFAULT_VALUE,
                void (*Interrupt_Function)() = nullptr);

    virtual bool begin(int32_t speed = DRIVEBUS_DEFAULT_VALUE);
    virtual int32_t IIC_Device_ID(void) = 0;

    virtual int32_t IIC_Device_Reset(void);
    // 写设备状态开关的虚函数
    virtual bool IIC_Write_Device_State(uint32_t device, uint8_t state);
    // 写设备值的虚函数
    virtual bool IIC_Write_Device_Value(uint32_t device, int64_t value);
    // 写连续数据的虚函数
    virtual bool IIC_Write_Device_Data(uint32_t device, const uint8_t *data, size_t length);
    // 读取状态信息虚函数
    virtual String IIC_Read_Device_State(uint32_t information);
    // 读取值信息虚函数
    virtual double IIC_Read_Device_Value(uint32_t information);

    // Flag
    volatile int8_t IIC_Interrupt_Flag = DRIVEBUS_DEFAULT_VALUE; // 放中断里的参数必须加volatile否则值容易被清除

protected:
    virtual bool IIC_Initialization(void) = 0;

    std::shared_ptr<Arduino_IIC_DriveBus> _bus;

    uint8_t _device_address;
    int8_t _rst, _iqr;

    void (*_Interrupt_Function)(){};
};