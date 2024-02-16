#pragma once

#define ADC1_CH_0 ADC1_CHANNEL_0

static float sensor_value = 0.0f;

void configure_adc();

float get_adc_sensor_value();

void xADC(void *arg);
