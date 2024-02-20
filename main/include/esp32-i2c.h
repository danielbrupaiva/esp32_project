#pragma once

#include "esp32-defines.h"

// i2c
#define I2C_SDA GPIO_NUM_12
#define I2C_SCL GPIO_NUM_14

#ifdef __cplusplus
extern "C" {
#endif

extern volatile mpu6050_acceleration_float_t acceleration;

extern volatile mpu6050_rotation_float_t gyroscope;

void configure_i2c(void);

void configure_mpu6050(void);

void configure_mpu6050(void);

mpu6050_acceleration_float_t get_mpu6050_accel_data(void);

mpu6050_rotation_float_t get_mpu6050_gyro_data(void);

void xMPU6050(void *arg);

#ifdef __cplusplus
}
#endif
