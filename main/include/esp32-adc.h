#pragma once

#include <driver/adc.h>

#include "esp32-defines.h"
// analog inputs
#define ADC1_CH_0 ADC_CHANNEL_0

#ifdef __cplusplus
extern "C" {
#endif

volatile adc_t adc_sensors[1];

void configure_adc();

float get_adc_sensor_value();

void xADC(void *arg);

#ifdef __cplusplus
}
#endif
