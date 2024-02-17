#pragma once

#include <driver/adc.h>

#include "esp32-defines.h"

extern volatile adc_t adc_sensors[1];

void configure_adc();

float get_adc_sensor_value();

void xADC(void *arg);
