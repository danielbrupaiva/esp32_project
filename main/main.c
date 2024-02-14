#define DEBUG 1

#include <string.h>
// Log
#include "esp_log.h"
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
#include "i2c.h"
#include "cJSON.h"
#include "cJSON_Utils.h"

void create_json(const uint32_t id,
                 const float adc_sensor_1,
                 const mpu6050_acceleration_t accel,
                 const mpu6050_rotation_t gyro)
{
    static const char *TAG = "cJSON";

    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "id", 1);

    cJSON *push_buttons = cJSON_CreateObject();
    cJSON_AddBoolToObject(push_buttons, "button_0", true);
    cJSON_AddBoolToObject(push_buttons, "button_1", true);
    cJSON_AddBoolToObject(push_buttons, "button_2", true);
    cJSON_AddBoolToObject(push_buttons, "button_3", true);
    cJSON_AddItemToObject(json, "push_buttons", push_buttons);

    cJSON *leds = cJSON_CreateObject();
    cJSON_AddBoolToObject(leds, "led_0", true);
    cJSON_AddBoolToObject(leds, "led_1", true);
    cJSON_AddItemToObject(json, "leds", leds);

    cJSON *adc_sensors = cJSON_CreateObject();
    cJSON_AddNumberToObject(adc_sensors, "adc_sensor_1", adc_sensor_1);
    cJSON_AddItemToObject(json, "adc_sensors", adc_sensors);

    cJSON *mpu6050_acceleration = cJSON_CreateObject();
    cJSON_AddNumberToObject(mpu6050_acceleration, "accel_x", accel.x);
    cJSON_AddNumberToObject(mpu6050_acceleration, "accel_y", accel.y);
    cJSON_AddNumberToObject(mpu6050_acceleration, "accel_z", accel.z);
    cJSON_AddItemToObject(json, "mpu6050_acceleration", mpu6050_acceleration);

    cJSON *mpu6050_rotation = cJSON_CreateObject();
    cJSON_AddNumberToObject(mpu6050_rotation, "gyro_x", gyro.x);
    cJSON_AddNumberToObject(mpu6050_rotation, "gyro_y", gyro.y);
    cJSON_AddNumberToObject(mpu6050_rotation, "gyro_z", gyro.z);
    cJSON_AddItemToObject(json, "mpu6050_rotation", mpu6050_rotation);

    char *json_str = cJSON_Print(json);
    ESP_LOGI(TAG, "%s", json_str);
    cJSON_Delete(json);
}

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
    wifi_config_t wifi_config = {
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
    static uint32_t id = 0;
    for (;;) {
        ++id;
        create_json(id, sensor_value, accel, gyro);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}