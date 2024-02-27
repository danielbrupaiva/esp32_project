#include "esp32-adc.h"
// FreeRTOS
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char *TAG = "ADC";

extern uint32_t acquisition_time_ms;

volatile adc_t adc_sensors[1] = {
    {
        .adc_channel = ADC_CHANNEL_0,
        .name = "LDR_sensor",
        .value = 0
    }
};

void configure_adc()
{
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_CH_0, ADC_ATTEN_DB_12);
}

float get_adc_sensor_value()
{
    return (float) adc1_get_raw(ADC1_CH_0) * 100 / 4095;
}

void xADC(void *arg)
{
    configure_adc();
    ESP_LOGI(TAG, " configuration done!");
    for (;;) {
        adc_sensors[0].value = (float) adc1_get_raw(ADC1_CH_0) * 100 / 4095;
//        ESP_LOGI(TAG, "Sensor value: %0.2f", adc_sensors[0].value);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}