#pragma once

#include <time.h>
#include <hal/gpio_types.h>
#include <hal/adc_types.h>
#include "mpu6050.h"

typedef struct _gpio_t
{
    gpio_num_t pin_number;
    gpio_mode_t mode;
    char name[20];
    bool state;
} gpio_t;

typedef struct _adc_t
{
    adc_channel_t adc_channel;
    char name[20];
    float value;
} adc_t;

typedef struct _mpu6050_acceleration_float_t
{
    float x;
    float y;
    float z;
} mpu6050_acceleration_float_t;

typedef struct _mpu6050_rotation_float_t
{
    float x;
    float y;
    float z;
} mpu6050_rotation_float_t;

typedef struct _timestamp_t
{
    struct tm timeinfo;
    struct timeval tv_now;
} timestamp_t;




