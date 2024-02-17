#include "esp32-gpio.h"

#include <esp_log.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

volatile gpio_t leds[2] = {
    {
        .pin_number = DIGITAL_OUTPUT_0,
        .mode = GPIO_MODE_OUTPUT,
        .name = "led_0",
        .state = false
    },
    {
        .pin_number = DIGITAL_OUTPUT_1,
        .mode = GPIO_MODE_OUTPUT,
        .name = "led_1",
        .state = false
    },
};

volatile gpio_t push_buttons[4] = {
    {
        .pin_number = DIGITAL_INPUT_0,
        .mode = GPIO_MODE_INPUT,
        .name = "push_button_0",
        .state = false
    },
    {
        .pin_number = DIGITAL_INPUT_1,
        .mode = GPIO_MODE_INPUT,
        .name = "push_button_1",
        .state = false
    },
    {
        .pin_number = DIGITAL_INPUT_2,
        .mode = GPIO_MODE_INPUT,
        .name = "push_button_2",
        .state = false
    },
    {
        .pin_number = DIGITAL_INPUT_3,
        .mode = GPIO_MODE_INPUT,
        .name = "push_button_3",
        .state = false
    }
};

void xLED(void *arg)
{
    static const char *TAG = "GPIO";
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

    for (;;) {
//        gpio_set_level(DIGITAL_OUTPUT_0, gpio_get_level(DIGITAL_INPUT_0));
//        gpio_set_level(DIGITAL_OUTPUT_1, gpio_get_level(DIGITAL_INPUT_1));
        gpio_set_level(DIGITAL_OUTPUT_0, leds[0].state);
        gpio_set_level(DIGITAL_OUTPUT_1, leds[1].state);
    }
    vTaskDelete(NULL);
}

void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

void xButton(void *arg)
{
    static const char *TAG = "GPIO";
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
    gpio_isr_handler_add(DIGITAL_INPUT_0, gpio_isr_handler, (void *) DIGITAL_INPUT_0);
    gpio_isr_handler_add(DIGITAL_INPUT_1, gpio_isr_handler, (void *) DIGITAL_INPUT_1);
    gpio_isr_handler_add(DIGITAL_INPUT_2, gpio_isr_handler, (void *) DIGITAL_INPUT_2);
    gpio_isr_handler_add(DIGITAL_INPUT_3, gpio_isr_handler, (void *) DIGITAL_INPUT_3);
    ESP_LOGI(TAG, "Buttons configured");

    static uint32_t pin_number;
    for (;;) {
        if (xQueueReceive(gpio_evt_queue, &pin_number, portMAX_DELAY)) {
            debounce(pin_number, debounce_time_ms, callbacks[pin_number]);
        }
    }
    vTaskDelete(NULL);
}

void debounce(uint32_t _pin_number, uint32_t _debounce_time, void (*pfHandler)(void))
{
    bool current_state = gpio_get_level(_pin_number);
    if (current_state) {
        gpio_isr_handler_remove(_pin_number);
        while (current_state == gpio_get_level(_pin_number)) {
            vTaskDelay(_debounce_time / portTICK_PERIOD_MS);
        }
        pfHandler();
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
    gpio_isr_handler_add(_pin_number, gpio_isr_handler, (void *) _pin_number);
}

void button_0_cb(void)
{
    static const char *TAG = "GPIO";
    static uint32_t counter = 0;
    ++counter;
    ESP_LOGI(TAG, "Button 0 pressed %d times", counter);
    push_buttons[0].state = !push_buttons[0].state;
    leds[0].state = push_buttons[0].state;
    ESP_LOGI(TAG, "Button 0 state %d", push_buttons[0].state);
}

void button_1_cb(void)
{
    static const char *TAG = "GPIO";
    static uint32_t counter = 0;
    ++counter;
    ESP_LOGI(TAG, "Button 1 pressed %d times", counter);
    push_buttons[1].state = !push_buttons[1].state;
    leds[1].state = push_buttons[1].state;
    ESP_LOGI(TAG, "Button 1 state %d", push_buttons[1].state);
}

void button_2_cb(void)
{
    static const char *TAG = "GPIO";
    static uint32_t counter = 0;
    ++counter;
    ESP_LOGI(TAG, "Button 2 pressed %d times", counter);
    push_buttons[2].state = !push_buttons[2].state;
    ESP_LOGI(TAG, "Button 2 state %d", push_buttons[2].state);
}

void button_3_cb(void)
{
    static const char *TAG = "GPIO";
    static uint32_t counter = 0;
    ++counter;
    ESP_LOGI(TAG, "Button 3 pressed %d times", counter);
    push_buttons[3].state = !push_buttons[3].state;
    ESP_LOGI(TAG, "Button 3 state %d", push_buttons[3].state);
}