#pragma once

#include <esp_log.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
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

typedef void (*pfHandler)(void);

static QueueHandle_t gpio_evt_queue = NULL;

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

