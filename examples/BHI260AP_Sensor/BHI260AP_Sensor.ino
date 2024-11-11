/**
 *
 * @license MIT License
 *
 * Copyright (c) 2023 lewis he
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file      BHI260AP_6DoF.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2023-09-06
 * @note      Changed from Boschsensortec API https://github.com/boschsensortec/BHY2_SensorAPI
 */
#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include "SensorBHI260AP.hpp"

#define SENSOR_SDA                  5
#define SENSOR_SCL                  6
#define SENSOR_IRQ                  21
#define SENSOR_RST                  -1

SensorBHI260AP bhy;

void accel_process_callback(uint8_t sensor_id, uint8_t *data_ptr, uint32_t len, uint64_t *timestamp)
{
    struct bhy2_data_xyz data;
    float scaling_factor = get_sensor_default_scaling(sensor_id);
    bhy2_parse_xyz(data_ptr, &data);
    Serial.print(bhy.getSensorName(sensor_id));
    Serial.print(":");
    Serial.printf("x: %f, y: %f, z: %f;\r\n",
                  data.x * scaling_factor,
                  data.y * scaling_factor,
                  data.z * scaling_factor
                 );
}


void gyro_process_callback(uint8_t sensor_id, uint8_t *data_ptr, uint32_t len, uint64_t *timestamp)
{
    struct bhy2_data_xyz data;
    float scaling_factor = get_sensor_default_scaling(sensor_id);
    bhy2_parse_xyz(data_ptr, &data);
    Serial.print(bhy.getSensorName(sensor_id));
    Serial.print(":");
    Serial.printf("x: %f, y: %f, z: %f;\r\n",
                  data.x * scaling_factor,
                  data.y * scaling_factor,
                  data.z * scaling_factor
                 );
}

void setup()
{
    Serial.begin(115200);
    while (!Serial);

    // Set the reset pin and interrupt pin, if any
    bhy.setPins(SENSOR_RST, SENSOR_IRQ);

    Serial.println("Initializing Sensors...");
    // Using I2C interface
    if (!bhy.init(Wire, SENSOR_SDA, SENSOR_SCL, BHI260AP_SLAVE_ADDRESS_L)) {
        Serial.print("Failed to initialize sensor - error code:");
        Serial.println(bhy.getError());
        while (1) {
            delay(1000);
        }
    }

    Serial.println("Initializing the sensor successfully!");

    // Output all available sensors to Serial
    bhy.printSensors(Serial);

    float sample_rate = 100.0;      /* Read out hintr_ctrl measured at 100Hz */
    uint32_t report_latency_ms = 0; /* Report immediately */

    // Enable acceleration
    bhy.configure(SENSOR_ID_ACC_PASS, sample_rate, report_latency_ms);
    // Enable gyroscope
    bhy.configure(SENSOR_ID_GYRO_PASS, sample_rate, report_latency_ms);

    // Set the acceleration sensor result callback function
    bhy.onResultEvent(SENSOR_ID_ACC_PASS, accel_process_callback);

    // Set the gyroscope sensor result callback function
    bhy.onResultEvent(SENSOR_ID_GYRO_PASS, gyro_process_callback);

}


void loop()
{
    // Update sensor fifo
    bhy.update();
    delay(50);
}



