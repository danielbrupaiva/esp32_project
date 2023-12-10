#pragma once

// Drivers
#include "driver/gpio.h"

// digital inputs
#define DIGITAL_INPUT_0 GPIO_NUM_35
#define DIGITAL_INPUT_1 GPIO_NUM_34
#define GPIO_INPUT_PIN_SEL ((1ULL << DIGITAL_INPUT_0) | (1ULL << DIGITAL_INPUT_1))

// digital outputs
#define DIGITAL_OUTPUT_0 GPIO_NUM_26
#define DIGITAL_OUTPUT_1 GPIO_NUM_27
#define GPIO_OUTPUT_PIN_SEL ((1ULL << DIGITAL_OUTPUT_0) | (1ULL << DIGITAL_OUTPUT_1))

static void button_0_cb(void);
static void button_1_cb(void);

typedef void (*pfHandler)(void);

pfHandler callbacks[] = {
    [DIGITAL_INPUT_0] = button_0_cb,
    [DIGITAL_INPUT_1] = button_1_cb};

static void debounce(uint32_t _pin_number, uint32_t _debounce_time, void (*pfHandler)(void));

static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}
static void button_0_cb(void)
{
    const char *TAG = "button_0_cb";
    static uint32_t counter1 = 0;
    counter1++;
    ESP_LOGI(TAG, "Button 1 pressed %d times", counter1);
}
static void button_1_cb(void)
{
    const char *TAG = "button_1_cb";
    static uint32_t counter2 = 0;
    counter2++;
    ESP_LOGI(TAG, "Button 2 pressed %d times", counter2);
}

static void debounce(uint32_t _pin_number, uint32_t _debounce_time, void (*pfHandler)(void))
{
    const char *TAG = "debounce";
    bool current_state = gpio_get_level(_pin_number);
    if (current_state)
    {
        gpio_isr_handler_remove(_pin_number);
        while (current_state == gpio_get_level(_pin_number))
        {
            vTaskDelay(_debounce_time / portTICK_PERIOD_MS);
        }
        pfHandler();
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
    gpio_isr_handler_add(_pin_number, gpio_isr_handler, (void *)_pin_number);
}

static void xButton(void *arg)
{
    const char *TAG = "xButton";
    // create a queue to handle gpio event from isr before install isr
    gpio_evt_queue = xQueueCreate(10, sizeof(int));

    // configure input port
    gpio_config_t input_conf = {
        .intr_type = GPIO_INTR_POSEDGE,       // enable interrupt
        .mode = GPIO_MODE_INPUT,              // set as output mode
        .pin_bit_mask = GPIO_INPUT_PIN_SEL,   // bit mask of the pins that you want to set,e.g.GPIO18/19
        .pull_down_en = GPIO_PULLDOWN_ENABLE, // enable pull-down mode
        .pull_up_en = GPIO_PULLUP_ENABLE,     // enable pull-up mode
    };
    gpio_config(&input_conf);
    // configure interrupt
    // gpio_set_intr_type(DIGITAL_INPUT_1, GPIO_INTR_ANYEDGE);
    // install gpio isr service
    gpio_install_isr_service(0);
    gpio_isr_handler_add(DIGITAL_INPUT_0, gpio_isr_handler, (void *)DIGITAL_INPUT_0);
    gpio_isr_handler_add(DIGITAL_INPUT_1, gpio_isr_handler, (void *)DIGITAL_INPUT_1);
    ESP_LOGI(TAG, "Buttons configured");

    uint32_t pin_number;

    for (;;)
    {
        if (xQueueReceive(gpio_evt_queue, &pin_number, portMAX_DELAY))
        {
            debounce(pin_number, 50, callbacks[pin_number]);
        }
    }
    vTaskDelete(NULL);
}
static void xLED(void *arg)
{
    const char *TAG = "xLED";
    // configure output port
    gpio_config_t output_conf = {
        .intr_type = GPIO_INTR_DISABLE,        // disable interrupt
        .mode = GPIO_MODE_OUTPUT,              // set as output mode
        .pin_bit_mask = GPIO_OUTPUT_PIN_SEL,   // bit mask of the pins that you want to set,e.g.GPIO18/19
        .pull_down_en = GPIO_PULLDOWN_DISABLE, // disable pull-down mode
        .pull_up_en = GPIO_PULLUP_DISABLE,     // disable pull-up mode
    };
    gpio_config(&output_conf);
    ESP_LOGI(TAG, "LEDs configured");

    for (;;)
    {
        gpio_set_level(DIGITAL_OUTPUT_0, gpio_get_level(DIGITAL_INPUT_0));
        gpio_set_level(DIGITAL_OUTPUT_1, gpio_get_level(DIGITAL_INPUT_1));
    }
    vTaskDelete(NULL);
}
