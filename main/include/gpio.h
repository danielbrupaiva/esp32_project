#pragma once

// Drivers
#include "driver/gpio.h"

static QueueHandle_t gpio_evt_queue = NULL;
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void xGPIO(void *arg)
{
    static const char *TAG = "xGPIO";
    ESP_LOGI(TAG, "Start");
    // configure input port
    gpio_config_t input_conf = {
        .intr_type = GPIO_INTR_DISABLE,      // disable interrupt
        .mode = GPIO_MODE_OUTPUT,            // set as output mode
        .pin_bit_mask = GPIO_OUTPUT_PIN_SEL, // bit mask of the pins that you want to set,e.g.GPIO18/19
        .pull_down_en = 0,                   // disable pull-down mode
        .pull_up_en = 0,                     // disable pull-up mode
    };
    gpio_config(&input_conf);
    // configure output port
    gpio_config_t output_conf = {
        .intr_type = GPIO_INTR_NEGEDGE,     // interrupt of negative edge
        .mode = GPIO_MODE_INPUT,            // set as input mode
        .pin_bit_mask = GPIO_INPUT_PIN_SEL, // bit mask of the pins, e.g use GPIO4/5 here
        .pull_down_en = 1,                  // disable pull-down mode
        .pull_up_en = 1,                    // enable pull-up mode

    };
    gpio_config(&output_conf);
    // configure interrupt
    // create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(int));
    // install gpio isr service
    gpio_install_isr_service(0);
    gpio_isr_handler_add(DIGITAL_INPUT_0, gpio_isr_handler, (void *)DIGITAL_INPUT_0);
    gpio_isr_handler_add(DIGITAL_INPUT_1, gpio_isr_handler, (void *)DIGITAL_INPUT_1);
    ESP_LOGI(TAG, "GPIOs configured");

    uint32_t pin_number;
    for (;;)
    {
        if (xQueueReceive(gpio_evt_queue, &pin_number, portMAX_DELAY))
        {
            switch (pin_number)
            {
            case DIGITAL_INPUT_0:
                gpio_isr_handler_remove(DIGITAL_INPUT_0);
                // call function
                gpio_isr_handler_add(DIGITAL_INPUT_0, gpio_isr_handler, (void *)DIGITAL_INPUT_0);
                break;
            case DIGITAL_INPUT_1:
                gpio_isr_handler_remove(DIGITAL_INPUT_1);
                // call function
                gpio_isr_handler_add(DIGITAL_INPUT_1, gpio_isr_handler, (void *)DIGITAL_INPUT_1);
                break;
            default:
                break;
            }
        }
    }

    vTaskDelete(NULL);
}