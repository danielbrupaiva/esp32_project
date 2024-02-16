#include "payload.h"

char *create_json_object(void)
{
    static const char *TAG = "Payload";
    cJSON *root = cJSON_CreateObject();
    if (!root) {
        ESP_LOGE(TAG, "Error creating JSON object.");
        return NULL;
    }

    cJSON *esp32_webserver = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "esp32-webserver", esp32_webserver);

    cJSON_AddNumberToObject(esp32_webserver, "id", ++id);
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
    cJSON_AddNumberToObject(adc_sensor0, "value", (float) 1.5);
    cJSON_AddItemToArray(adc_sensors_array, adc_sensor0);

    // Add other ADC sensor objects similarly

    cJSON *mpu6050_object = cJSON_CreateObject();
    cJSON_AddItemToObject(esp32_webserver, "mpu6050", mpu6050_object);

    cJSON *accel_object = cJSON_CreateObject();
    cJSON_AddItemToObject(mpu6050_object, "accel", accel_object);
    cJSON_AddNumberToObject(accel_object, "x", (float) 0.0);
    cJSON_AddNumberToObject(accel_object, "y", (float) 0.0);
    cJSON_AddNumberToObject(accel_object, "z", (float) 0.0);

    cJSON *gyro_object = cJSON_CreateObject();
    cJSON_AddItemToObject(mpu6050_object, "gyro", gyro_object);
    cJSON_AddNumberToObject(gyro_object, "x", (float) 0.0);
    cJSON_AddNumberToObject(gyro_object, "y", (float) 0.0);
    cJSON_AddNumberToObject(gyro_object, "z", (float) 0.0);

    char *json_str = cJSON_Print(root);
    ESP_LOGI(TAG, "%s", json_str);

    cJSON_Delete(root);
    return json_str;
}