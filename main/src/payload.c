#include "payload.h"

#include "timestamp.h"
#include <esp_log.h>
#include "cJSON.h"

extern gpio_t leds[2];

extern gpio_t push_buttons[4];

extern adc_t adc_sensors[1];

extern volatile mpu6050_acceleration_float_t acceleration;

extern volatile mpu6050_rotation_float_t gyroscope;

char *create_json_object(timestamp_t *timestamp, char *strftime_buf)
{
    static const char *TAG = "Payload";
    cJSON *root = cJSON_CreateObject();
    if (!root) {
        ESP_LOGE(TAG, "Error creating JSON object.");
        return NULL;
    }

    cJSON *esp32_webserver = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "esp32-webserver", esp32_webserver);

    cJSON_AddNumberToObject(esp32_webserver, "id", ++payload_id);
    char buffer[120];
    sprintf(buffer, "%s.%lld", strftime_buf, (uint64_t) timestamp->tv_now.tv_usec);
    cJSON_AddStringToObject(esp32_webserver, "timestamp", (char *) buffer);

    cJSON *leds_array = cJSON_CreateArray();
    cJSON_AddItemToObject(esp32_webserver, "leds", leds_array);

    cJSON *led0 = cJSON_CreateObject();
    cJSON_AddNumberToObject(led0, "id", leds[0].pin_number);
    cJSON_AddStringToObject(led0, "name", leds[0].name);
    cJSON_AddBoolToObject(led0, "value", leds[0].state);
    cJSON_AddItemToArray(leds_array, led0);

    cJSON *led1 = cJSON_CreateObject();
    cJSON_AddNumberToObject(led1, "id", leds[1].pin_number);
    cJSON_AddStringToObject(led1, "name", leds[1].name);
    cJSON_AddBoolToObject(led1, "value", leds[1].state);
    cJSON_AddItemToArray(leds_array, led1);

    cJSON *push_buttons_array = cJSON_CreateArray();
    cJSON_AddItemToObject(esp32_webserver, "push_buttons", push_buttons_array);

    cJSON *push_button0 = cJSON_CreateObject();
    cJSON_AddNumberToObject(push_button0, "id", push_buttons[0].pin_number);
    cJSON_AddStringToObject(push_button0, "name", push_buttons[0].name);
    cJSON_AddBoolToObject(push_button0, "value", push_buttons[0].state);
    cJSON_AddItemToArray(push_buttons_array, push_button0);

    cJSON *push_button1 = cJSON_CreateObject();
    cJSON_AddNumberToObject(push_button1, "id", push_buttons[1].pin_number);
    cJSON_AddStringToObject(push_button1, "name", push_buttons[1].name);
    cJSON_AddBoolToObject(push_button1, "value", push_buttons[1].state);
    cJSON_AddItemToArray(push_buttons_array, push_button1);

    cJSON *push_button2 = cJSON_CreateObject();
    cJSON_AddNumberToObject(push_button2, "id", push_buttons[2].pin_number);
    cJSON_AddStringToObject(push_button2, "name", push_buttons[2].name);
    cJSON_AddBoolToObject(push_button2, "value", push_buttons[2].state);
    cJSON_AddItemToArray(push_buttons_array, push_button2);

    cJSON *push_button3 = cJSON_CreateObject();
    cJSON_AddNumberToObject(push_button3, "id", push_buttons[3].pin_number);
    cJSON_AddStringToObject(push_button3, "name", push_buttons[3].name);
    cJSON_AddBoolToObject(push_button3, "value", push_buttons[3].state);
    cJSON_AddItemToArray(push_buttons_array, push_button3);

    cJSON *adc_sensors_array = cJSON_CreateArray();
    cJSON_AddItemToObject(esp32_webserver, "adc_sensors", adc_sensors_array);

    cJSON *adc_sensor0 = cJSON_CreateObject();
    cJSON_AddNumberToObject(adc_sensor0, "id", adc_sensors[0].adc_channel);
    cJSON_AddStringToObject(adc_sensor0, "name", adc_sensors[0].name);
    cJSON_AddNumberToObject(adc_sensor0, "value", adc_sensors[0].value);
    cJSON_AddItemToArray(adc_sensors_array, adc_sensor0);

    // Add other ADC sensor objects similarly

    cJSON *mpu6050_object = cJSON_CreateObject();
    cJSON_AddItemToObject(esp32_webserver, "mpu6050", mpu6050_object);

    cJSON *accel_object = cJSON_CreateObject();
    cJSON_AddItemToObject(mpu6050_object, "accel", accel_object);
    cJSON_AddNumberToObject(accel_object, "x", acceleration.x);
    cJSON_AddNumberToObject(accel_object, "y", acceleration.y);
    cJSON_AddNumberToObject(accel_object, "z", acceleration.z);

    cJSON *gyro_object = cJSON_CreateObject();
    cJSON_AddItemToObject(mpu6050_object, "gyro", gyro_object);
    cJSON_AddNumberToObject(gyro_object, "x", gyroscope.x);
    cJSON_AddNumberToObject(gyro_object, "y", gyroscope.y);
    cJSON_AddNumberToObject(gyro_object, "z", gyroscope.z);

    char *json_str = cJSON_Print(root);
    ESP_LOGI(TAG, "%s", json_str);
    cJSON_Delete(root);
    return json_str;

}

void xPayload(void *arg)
{
    static const char *TAG = "Payload";

    initialize_sntp();

    for (;;) {

        timestamp_t timestamp = get_timestamp();
        char strftime_buf[120];
        strftime(strftime_buf, sizeof(strftime_buf), "%Y-%m-%d %H:%M:%S", &timestamp.timeinfo);
        ESP_LOGI(TAG, "%s.%lld", strftime_buf, (uint64_t) timestamp.tv_now.tv_usec);
        create_json_object(&timestamp, strftime_buf);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}