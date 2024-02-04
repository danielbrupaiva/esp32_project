#define DEBUG 1

#include <stdio.h>
#include <string.h>
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

#include "gpio.h"
#include "adc.h"
#include "wifi.h"

static SemaphoreHandle_t xSemaphore1 = NULL;
static SemaphoreHandle_t xSemaphore2 = NULL;
static SemaphoreHandle_t xSemaphore3 = NULL;

static const char *TAG = "MAIN";
void app_main(void)
{
    ESP_LOGI(TAG, "Start");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /** Deploy tasks **/
    if (pdTRUE != xTaskCreate(xLED, "Task LED", configMINIMAL_STACK_SIZE + 2048, NULL, 0, NULL))
    {
        ESP_LOGI(TAG, "error - nao foi possivel alocar Task LED.");
        return;
    }
    if (pdTRUE != xTaskCreate(xButton, "Task Button", configMINIMAL_STACK_SIZE + 2048, NULL, 0, NULL))
    {
        ESP_LOGI(TAG, "error - nao foi possivel alocar Task Button.");
        return;
    }
    /*
    if (pdTRUE != xTaskCreate(xADC, "Task ADC", configMINIMAL_STACK_SIZE + 2048, NULL, 0, NULL))
    {
        ESP_LOGI(TAG, "error - nao foi possivel alocar Task ADC.");
        return;
    }*/

    configure_start_wifi();
    
    
}
