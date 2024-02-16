#pragma once

#include <esp_log.h>
#include "mpu6050.h"

#define I2C_SDA GPIO_NUM_12
#define I2C_SCL GPIO_NUM_14

void configure_i2c(void);

void configure_mpu6050(void);

void configure_mpu6050(void);

mpu6050_acceleration_t get_mpu6050_accel_data(void);

mpu6050_rotation_t get_mpu6050_gyro_data(void);

void xMPU6050(void *arg);
