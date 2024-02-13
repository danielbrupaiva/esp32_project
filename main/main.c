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
#include "mpu6050.h"

#define I2C_SDA GPIO_NUM_12
#define I2C_SCL GPIO_NUM_14
static void xMPU6050(void *arg);

static float accel_x, accel_y, accel_z;

static float gyro_x, gyro_y, gyro_z;

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

    if (xTaskCreate(xMPU6050, "Task MPU6050", 2048, NULL, 0, NULL) != pdTRUE) {
        ESP_LOGI(TAG, "error - nao foi possivel alocar Task MPU6050.");
        return;
    }
}

static void xMPU6050(void *arg)
{
    static const char *TAG = "MPU6050";
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA, // select GPIO specific to your project
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_SCL, // select GPIO specific to your project
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000, // select frequency specific to your project
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));
    ESP_LOGI(TAG, "I2C initialized successfully");

    // mpu6050_reset();
    // ESP_LOGI(TAG, "reset successfully");
    mpu6050_init();

    while (!mpu6050_test_connection()) {}

    if (mpu6050_test_connection()) {
        ESP_LOGI(TAG, "mpu detected");
    }

    mpu6050_acceleration_t raw_acc_data = {0};
    mpu6050_rotation_t raw_gyro_data = {0};

    float acc_scale_factor = 16384.0;
    float gyro_scale_factor = 131.0;

    for (;;) {

        mpu6050_get_acceleration(&raw_acc_data);
        mpu6050_get_rotation(&raw_gyro_data);

        accel_x = (float) raw_acc_data.accel_x / acc_scale_factor;
        accel_y = (float) raw_acc_data.accel_y / acc_scale_factor;
        accel_z = (float) raw_acc_data.accel_z / acc_scale_factor;
        ESP_LOGI(TAG, "accel_x: %0.2f | accel_y: %0.2f | accel_z: %0.2f", accel_x, accel_y, accel_z);
        gyro_x = (float) raw_gyro_data.gyro_x / gyro_scale_factor;
        gyro_y = (float) raw_gyro_data.gyro_y / gyro_scale_factor;
        gyro_z = (float) raw_gyro_data.gyro_z / gyro_scale_factor;
        ESP_LOGI(TAG, "gyro_x: %0.2f | gyro_y: %0.2f | gyro_z: %0.2f", gyro_x, gyro_y, gyro_z);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}