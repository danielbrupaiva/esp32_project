#pragma once
// analog inputs
#define ADC1_CH_0 ADC1_CHANNEL_5

#include "driver/adc.h"

void xADC(void *arg)
{
    static const char *TAG = "xADC";
    ESP_LOGI(TAG, "Start");

    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_CH_0, ADC_ATTEN_DB_11);

    float sensor_value = 0.0f;
    for (;;)
    {
        sensor_value = (float)adc1_get_raw(ADC1_CH_0) * 100 / 4095;
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}
