#pragma once
// Log
#include "esp_log.h"
#include "freertos/task.h"
#include "driver/adc.h"
// analog inputs
#define ADC1_CH_0 ADC1_CHANNEL_0

static float sensor_value = 0.0f;

void xADC(void *arg)
{
    static const char *TAG = "ADC";
    ESP_LOGI(TAG, "Start");

    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_CH_0, ADC_ATTEN_DB_12);

    for (;;) {
        sensor_value = (float) adc1_get_raw(ADC1_CH_0) * 100 / 4095;
        ESP_LOGI(TAG, "Sensor value: %0.2f", sensor_value);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}
