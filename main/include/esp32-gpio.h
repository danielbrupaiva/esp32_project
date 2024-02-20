#pragma once

#include "esp32-defines.h"

// digital inputs
#define DIGITAL_INPUT_0 GPIO_NUM_32
#define DIGITAL_INPUT_1 GPIO_NUM_35
#define DIGITAL_INPUT_2 GPIO_NUM_34
#define DIGITAL_INPUT_3 GPIO_NUM_33
#define GPIO_INPUT_PIN_SEL ( (1ULL << DIGITAL_INPUT_0) | (1ULL << DIGITAL_INPUT_1) | (1ULL << DIGITAL_INPUT_2) | (1ULL << DIGITAL_INPUT_3)  )
// digital outputs
#define DIGITAL_OUTPUT_0 GPIO_NUM_22
#define DIGITAL_OUTPUT_1 GPIO_NUM_23
#define DIGITAL_OUTPUT_3 GPIO_NUM_
#define GPIO_OUTPUT_PIN_SEL ( (1ULL << DIGITAL_OUTPUT_0) | (1ULL << DIGITAL_OUTPUT_1) )

#ifdef __cplusplus
extern "C" {
#endif

volatile gpio_t leds[2];

volatile gpio_t push_buttons[4];

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

#ifdef __cplusplus
}
#endif

