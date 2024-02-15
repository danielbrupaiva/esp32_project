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

#include "timestamp.h"

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

cJSON *create_json_object()
{
    cJSON *root = cJSON_CreateObject();
    if (!root) {
        printf("Error creating JSON object\n");
        return NULL;
    }

    cJSON *esp32_webserver = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "esp32-webserver", esp32_webserver);

    cJSON_AddNumberToObject(esp32_webserver, "id", 0);
    cJSON_AddStringToObject(esp32_webserver, "timestamp", "hh:mm:ss.sss");

    cJSON *leds_array = cJSON_CreateArray();
    cJSON_AddItemToObject(esp32_webserver, "leds", leds_array);

    cJSON *led0 = cJSON_CreateObject();
    cJSON_AddNumberToObject(led0, "id", 0);
    cJSON_AddStringToObject(led0, "name", "led_0");
    cJSON_AddBoolToObject(led0, "value", true);
    cJSON_AddItemToArray(leds_array, led0);

    cJSON *led1 = cJSON_CreateObject();
    cJSON_AddNumberToObject(led1, "id", 1);
    cJSON_AddStringToObject(led1, "name", "led_1");
    cJSON_AddBoolToObject(led1, "value", true);
    cJSON_AddItemToArray(leds_array, led1);

    cJSON *push_buttons_array = cJSON_CreateArray();
    cJSON_AddItemToObject(esp32_webserver, "push_buttons", push_buttons_array);

    cJSON *push_button0 = cJSON_CreateObject();
    cJSON_AddNumberToObject(push_button0, "id", 0);
    cJSON_AddStringToObject(push_button0, "name", "push_button_0");
    cJSON_AddBoolToObject(push_button0, "value", true);
    cJSON_AddItemToArray(push_buttons_array, push_button0);

    cJSON *push_button1 = cJSON_CreateObject();
    cJSON_AddNumberToObject(push_button1, "id", 1);
    cJSON_AddStringToObject(push_button1, "name", "push_button_1");
    cJSON_AddBoolToObject(push_button1, "value", true);
    cJSON_AddItemToArray(push_buttons_array, push_button1);

    cJSON *push_button2 = cJSON_CreateObject();
    cJSON_AddNumberToObject(push_button2, "id", 2);
    cJSON_AddStringToObject(push_button2, "name", "push_button_2");
    cJSON_AddBoolToObject(push_button2, "value", true);
    cJSON_AddItemToArray(push_buttons_array, push_button2);

    cJSON *push_button3 = cJSON_CreateObject();
    cJSON_AddNumberToObject(push_button3, "id", 3);
    cJSON_AddStringToObject(push_button3, "name", "push_button_3");
    cJSON_AddBoolToObject(push_button3, "value", true);
    cJSON_AddItemToArray(push_buttons_array, push_button3);

    cJSON *adc_sensors_array = cJSON_CreateArray();
    cJSON_AddItemToObject(esp32_webserver, "adc_sensors", adc_sensors_array);

    cJSON *adc_sensor0 = cJSON_CreateObject();
    cJSON_AddNumberToObject(adc_sensor0, "id", 0);
    cJSON_AddStringToObject(adc_sensor0, "name", "adc_sensor_0");
    cJSON_AddNumberToObject(adc_sensor0, "value", 1234.567);
    cJSON_AddItemToArray(adc_sensors_array, adc_sensor0);

    // Add other ADC sensor objects similarly

    cJSON *mpu6050_object = cJSON_CreateObject();
    cJSON_AddItemToObject(esp32_webserver, "mpu6050", mpu6050_object);

    cJSON *accel_object = cJSON_CreateObject();
    cJSON_AddItemToObject(mpu6050_object, "accel", accel_object);
    cJSON_AddNumberToObject(accel_object, "x", 1234.567);
    cJSON_AddNumberToObject(accel_object, "y", 1234.567);
    cJSON_AddNumberToObject(accel_object, "z", 1234.567);

    cJSON *gyro_object = cJSON_CreateObject();
    cJSON_AddItemToObject(mpu6050_object, "gyro", gyro_object);
    cJSON_AddNumberToObject(gyro_object, "x", 1234.567);
    cJSON_AddNumberToObject(gyro_object, "y", 1234.567);
    cJSON_AddNumberToObject(gyro_object, "z", 1234.567);

    return root;
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

    if (pdTRUE != xTaskCreate(xTimeStamp, "Task TimeStamp", configMINIMAL_STACK_SIZE + 2048, NULL, 1, NULL)) {
        ESP_LOGI(TAG, "error - nao foi possivel alocar Task TimeStamp.");
        return;
    }

}