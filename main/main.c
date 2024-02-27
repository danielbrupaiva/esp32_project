// Log
#include <esp_log.h>
// NVS
#include <nvs.h>
#include <nvs_flash.h>
// FreeRTOS
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
// Project files
#include "esp32-gpio.h"
#include "esp32-adc.h"
#include "esp32-i2c.h"
#include "esp32-wifi.h"
#include "payload.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "Start");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /** Configure wifi mode and wait until connection is ESTABLISH**/
    wifi_configure(WIFI_MODE_STA, &wifi_config);
    /** Synchronize internal clock **/
    initialize_sntp();
    /** Deploy tasks **/
    if (pdTRUE != xTaskCreate(xLED, "Task LED", configMINIMAL_STACK_SIZE + 2048, NULL, 0, NULL)) {
        ESP_LOGI(TAG, "error - nao foi possivel alocar Task LED.");
        return;
    }
    if (pdTRUE != xTaskCreate(xButton, "Task Button", configMINIMAL_STACK_SIZE + 2048, NULL, 0, NULL)) {
        ESP_LOGI(TAG, "error - nao foi possivel alocar Task Button.");
        return;
    }
    if (pdTRUE != xTaskCreate(xADC, "Task ADC", configMINIMAL_STACK_SIZE + 2048, NULL, 0, NULL)) {
        ESP_LOGI(TAG, "error - nao foi possivel alocar Task ADC.");
        return;
    }
    if (pdTRUE != xTaskCreate(xMPU6050, "Task MPU6050", configMINIMAL_STACK_SIZE + 2048, NULL, 0, NULL)) {
        ESP_LOGI(TAG, "error - nao foi possivel alocar Task MPU6050.");
        return;
    }
}
