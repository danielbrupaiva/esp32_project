#pragma once

#include <time.h>
#include <hal/gpio_types.h>
#include <hal/adc_types.h>
#include "mpu6050.h"

// digital inputs
#define DIGITAL_INPUT_0 GPIO_NUM_32
#define DIGITAL_INPUT_1 GPIO_NUM_35
#define DIGITAL_INPUT_2 GPIO_NUM_34
#define DIGITAL_INPUT_3 GPIO_NUM_33
#define GPIO_INPUT_PIN_SEL ( (1ULL << DIGITAL_INPUT_0) | (1ULL << DIGITAL_INPUT_1) | (1ULL << DIGITAL_INPUT_2) | (1ULL << DIGITAL_INPUT_3)  )
// digital outputs
#define DIGITAL_OUTPUT_0 GPIO_NUM_23
#define DIGITAL_OUTPUT_1 GPIO_NUM_22
#define DIGITAL_OUTPUT_3 GPIO_NUM_
#define GPIO_OUTPUT_PIN_SEL ( (1ULL << DIGITAL_OUTPUT_0) | (1ULL << DIGITAL_OUTPUT_1) )
// analog inputs
#define ADC1_CH_0 ADC_CHANNEL_0
// i2c
#define I2C_SDA GPIO_NUM_12
#define I2C_SCL GPIO_NUM_14

/** GLOBALS VARIABLES **/
static uint32_t acquisition_time_ms = 100 / portTICK_PERIOD_MS;

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




