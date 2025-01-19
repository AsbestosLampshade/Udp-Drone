#ifndef GYRO_H
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"

#define MPU_I2C i2c0
#define MPU_I2C_SCL_GPIO 21
#define MPU_I2C_SCL_PIN 27
#define MPU_I2C_SDA_GPIO 20
#define MPU_I2C_SDA_PIN 26

#define MPU_ADDR 0x68
#define WHO_AM_I_REG	0x75
#define TEMP_MSB 0x41
#define TEMP_LSB 0x42
#define PWR_MGMT0 0x6B
#define AFS 0x1C
#define FS 0x1B
#define GYRO 0x43
#define ACCEL 0x3B
#define CONFIG 0x1A


typedef struct{
    signed short val1;
    signed short val2;
    signed short val3;
}returner;

void i2c_begin();
bool timer_callback(__unused struct repeating_timer *t);
void timed_accel_gyro();
returner i2c_accel();
returner i2c_gyro();
#endif