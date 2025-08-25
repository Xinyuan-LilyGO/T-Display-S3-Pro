/*
 * @Description: Arduino_IIC.cpp
 * @version: V1.1.5
 * @Author: LILYGO_L
 * @Date: 2023-11-16 16:58:05
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2024-12-12 10:48:27
 * @License: GPL 3.0
 */
#include "Arduino_IIC.h"

Arduino_IIC::Arduino_IIC(std::shared_ptr<Arduino_IIC_DriveBus> bus, uint8_t device_address,
                         int8_t rst, int8_t iqr, void (*Interrupt_Function)())
    : _bus(bus), _device_address(device_address), _rst(rst), _iqr(iqr),
      _Interrupt_Function(Interrupt_Function)
{
}

bool Arduino_IIC::begin(int32_t speed)
{
    // if (_iqr != DRIVEBUS_DEFAULT_VALUE)
    // {
    //     static thread_local int8_t *p;
    //     p = &IIC_Interrupt_Flag;
    //     attachInterrupt(
    //         _iqr, []
    //         { *p = true; },
    //         FALLING);
    // }
    if (_iqr != DRIVEBUS_DEFAULT_VALUE)
    {
        if (_Interrupt_Function != nullptr)
        {
            pinMode(_iqr, INPUT_PULLUP);
            attachInterrupt(_iqr, (*_Interrupt_Function), FALLING);
        }
    }
    if (_bus->begin(speed) == false)
    {
        log_e("->_bus->begin(speed) fail");
        return false;
    }

    if (IIC_Initialization() == false)
    {
        log_e("->IIC_Initialization() fail");
        return false;
    }

    return true;
}

int32_t Arduino_IIC::IIC_Device_Reset(void)
{
    log_e("No 'IIC_Control_Device' fictional function has been created.");
    return -1;
}
bool Arduino_IIC::IIC_Write_Device_State(uint32_t device, uint8_t state)
{
    log_e("No 'IIC_Control_Device' fictional function has been created.");
    return -1;
}
bool Arduino_IIC::IIC_Write_Device_Value(uint32_t device, int64_t value)
{
    log_e("No 'IIC_Control_Device' fictional function has been created.");
    return -1;
}
bool Arduino_IIC::IIC_Write_Device_Data(uint32_t device, const uint8_t *data, size_t length)
{
    log_e("No 'IIC_Control_Device' fictional function has been created.");
    return -1;
}
String Arduino_IIC::IIC_Read_Device_State(uint32_t information)
{
    log_e("No 'IIC_Read_Information' fictional function has been created.");
    return "No 'IIC_Read_Information' fictional function has been created.";
}
double Arduino_IIC::IIC_Read_Device_Value(uint32_t information)
{
    log_e("No 'IIC_Read_Information' fictional function has been created.");
    return -1;
}
