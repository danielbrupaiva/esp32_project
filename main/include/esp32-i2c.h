#pragma once

#include "esp32-defines.h"

extern volatile mpu6050_acceleration_float_t acceleration;

extern volatile mpu6050_rotation_float_t gyroscope;

void configure_i2c(void);

void configure_mpu6050(void);

void configure_mpu6050(void);

mpu6050_acceleration_float_t get_mpu6050_accel_data(void);

mpu6050_rotation_float_t get_mpu6050_gyro_data(void);

void xMPU6050(void *arg);
