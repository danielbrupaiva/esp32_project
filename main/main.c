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
//#include "adc.h"
#include "wifi.h"

#include "htth_server.h"

void app_main(void)
{
    static const char *TAG = "MAIN";
    ESP_LOGI(TAG, "Start");

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    /** Configure wifi mode and wait until connection is ESTABLISH**/
    configure_wifi(WIFI_MODE_STA);
    static httpd_handle_t server = NULL;
    //ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED, &connect_handler, &server));
    //ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));
    start_webserver();
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

    /*if (pdTRUE != xTaskCreate(xADC, "Task ADC", configMINIMAL_STACK_SIZE + 2048, NULL, 0, NULL))
    {
        ESP_LOGI(TAG, "error - nao foi possivel alocar Task ADC.");
        return;
    }*/

}
