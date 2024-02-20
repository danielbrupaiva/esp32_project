#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <time.h>
#include <hal/gpio_types.h>
#include <hal/adc_types.h>
#include "mpu6050.h"


/* EventGroups */
static EventGroupHandle_t wifi_event_group;

#define WIFI_CONNECTED_BIT  BIT0
#define      WIFI_FAIL_BIT  BIT1

/* Wifi */
#define ESP_WIFI_CHANNEL CONFIG_ESP_WIFI_CHANNEL
#define ESP_MAX_STA_CONN CONFIG_ESP_MAX_STA_CONN

#define ESP_WIFI_SSID CONFIG_ESP_WIFI_SSID
#define ESP_WIFI_PASS CONFIG_ESP_WIFI_PASSWORD
#define ESP_MAXIMUM_RETRY CONFIG_ESP_MAXIMUM_RETRY

#if CONFIG_ESP_WIFI_AUTH_OPEN
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#elif CONFIG_ESP_WIFI_AUTH_WEP
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP
#elif CONFIG_ESP_WIFI_AUTH_WPA_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WAPI_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK
#endif

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




