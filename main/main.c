#define DEBUG 1

#include <string.h>
// Log
#include <esp_log.h>
#include <esp_event.h>
// NVS
#include <nvs.h>
#include <nvs_flash.h>
// FreeRTOS
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/event_groups.h>

#include "esp32-gpio.h"
#include "esp32-adc.h"
#include "esp32-wifi.h"
#include "esp32-i2c.h"
#include "timestamp.h"
#include "payload.h"


void app_main(void)
{
    static const char *TAG = "MAIN";
    ESP_LOGI(TAG, "Start");

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    /** Configure wifi mode and wait until connection is ESTABLISH**/
    static const wifi_config_t wifi_config = {
        .ap = {
            .ssid = ESP_WIFI_SSID,
            .ssid_len = strlen(ESP_WIFI_SSID),
            .channel = ESP_WIFI_CHANNEL,
            .password = ESP_WIFI_PASS,
            .max_connection = ESP_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
        .sta = {
            .ssid = ESP_WIFI_SSID,
            .password = ESP_WIFI_PASS,
            .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
        }
    };
    wifi_configure(WIFI_MODE_STA, &wifi_config);

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

    if (pdTRUE != xTaskCreate(xTimeStamp, "Task TimeStamp", configMINIMAL_STACK_SIZE + 2048, NULL, 1, NULL)) {
        ESP_LOGI(TAG, "error - nao foi possivel alocar Task TimeStamp.");
        return;
    }
}
