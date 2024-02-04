#pragma once
// digital inputs
#define DIGITAL_INPUT_0 GPIO_NUM_33
#define DIGITAL_INPUT_1 GPIO_NUM_32
#define DIGITAL_INPUT_2 GPIO_NUM_35
#define DIGITAL_INPUT_3 GPIO_NUM_34
#define GPIO_INPUT_PIN_SEL ( (1ULL << DIGITAL_INPUT_0) | (1ULL << DIGITAL_INPUT_1) | (1ULL << DIGITAL_INPUT_2) | (1ULL << DIGITAL_INPUT_3)  )

// digital outputs
#define DIGITAL_OUTPUT_0 GPIO_NUM_26
#define DIGITAL_OUTPUT_1 GPIO_NUM_27
#define GPIO_OUTPUT_PIN_SEL ((1ULL << DIGITAL_OUTPUT_0) | (1ULL << DIGITAL_OUTPUT_1))

// Drivers
#include "driver/gpio.h"

typedef void (*pfHandler)(void);

static QueueHandle_t gpio_evt_queue = NULL;
static void IRAM_ATTR gpio_isr_handler(void *arg);
static void xButton(void *arg);
static void xLED(void *arg);

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

/** Implementations **/

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}
static void debounce(uint32_t _pin_number, uint32_t _debounce_time, void (*pfHandler)(void))
{
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
static void button_0_cb(void)
{
    static const char *TAG = "button_0_cb";
    static uint32_t counter = 0;
    ++counter;
    ESP_LOGI(TAG, "Button 1 pressed %d times", counter);
}
static void button_1_cb(void)
{
    static const char *TAG = "button_1_cb";
    static uint32_t counter = 0;
    ++counter;
    ESP_LOGI(TAG, "Button 2 pressed %d times", counter);
}
static void button_2_cb(void)
{
  static const char *TAG = "button_2_cb";
  static uint32_t counter = 0;
  ++counter;
  ESP_LOGI(TAG, "Button 3 pressed %d times", counter);
}
static void button_3_cb(void)
{
  static const char *TAG = "button_3_cb";
  static uint32_t counter = 0;
  ++counter;
  ESP_LOGI(TAG, "Button 4 pressed %d times", counter);
}

static void xButton(void *arg)
{
    static const char *TAG = "xButton";
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
    gpio_isr_handler_add(DIGITAL_INPUT_2, gpio_isr_handler, (void *)DIGITAL_INPUT_2);
    gpio_isr_handler_add(DIGITAL_INPUT_3, gpio_isr_handler, (void *)DIGITAL_INPUT_3);
    ESP_LOGI(TAG, "Buttons configured");

    static uint32_t pin_number;
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
    static const char *TAG = "xLED";
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
        gpio_set_level(DIGITAL_OUTPUT_0, gpio_get_level(DIGITAL_INPUT_2));
        gpio_set_level(DIGITAL_OUTPUT_1, gpio_get_level(DIGITAL_INPUT_3));
    }
    vTaskDelete(NULL);
}