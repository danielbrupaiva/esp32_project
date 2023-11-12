#include "defines.h"

#include <stdio.h>
// Log
#include "esp_log.h"
#include "esp_system.h"
#include "esp_event.h"
// NVS
#include "nvs.h"
#include "nvs_flash.h"
// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
// Drivers
#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/i2c.h"

static const char* TAG = "MAIN"; 

static QueueHandle_t gpio_evt_queue = NULL;
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void xGPIO(void *arg)
{
    static const char* TAG = "xGPIO"; 
    ESP_LOGI(TAG,"Start");
    // configure input
    gpio_config_t input_conf = {
        .intr_type = GPIO_INTR_DISABLE,      // disable interrupt
        .mode = GPIO_MODE_OUTPUT,            // set as output mode
        .pin_bit_mask = GPIO_OUTPUT_PIN_SEL, // bit mask of the pins that you want to set,e.g.GPIO18/19
        .pull_down_en = 0,                   // disable pull-down mode
        .pull_up_en = 0,                     // disable pull-up mode
    };
    gpio_config(&input_conf);  
    // configure output 
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
    ESP_LOGI(TAG,"GPIOs configured");

    static uint32_t io_num;
    for(;;){

        //gpio_set_level(DIGITAL_OUTPUT_0, true);
        //gpio_set_level(DIGITAL_OUTPUT_1, false);
        //vTaskDelay(1000 / portTICK_PERIOD_MS);
        //gpio_set_level(DIGITAL_OUTPUT_0, false);
        //gpio_set_level(DIGITAL_OUTPUT_1, true);
        //vTaskDelay(1000 / portTICK_PERIOD_MS);

    }
    
    vTaskDelete(NULL);
}
void app_main(void)
{
    ESP_LOGI(TAG,"Start");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    // Register threads

    if ( pdTRUE != xTaskCreate(xGPIO, "Task GPIO", 2048, NULL, 0, NULL) )
    {
        ESP_LOGI(TAG, "error - nao foi possivel alocar Task GPIO.");
        return;
    }
    
}
