#pragma once

#include "esp32-defines.h"

extern volatile gpio_t leds[2];

extern volatile gpio_t push_buttons[4];

typedef void (*pfHandler)(void);

static QueueHandle_t gpio_evt_queue = NULL;

static const uint32_t debounce_time_ms = 50;

static void IRAM_ATTR gpio_isr_handler(void *arg);
void xButton(void *arg);
void xLED(void *arg);

static void debounce(uint32_t _pin_number, uint32_t _debounce_time, void (*pfHandler)(void));
static void button_0_cb(void);
static void button_1_cb(void);
static void button_2_cb(void);
static void button_3_cb(void);

static pfHandler callbacks[] = {
    [DIGITAL_INPUT_0] = button_0_cb,
    [DIGITAL_INPUT_1] = button_1_cb,
    [DIGITAL_INPUT_2] = button_2_cb,
    [DIGITAL_INPUT_3] = button_3_cb
};

