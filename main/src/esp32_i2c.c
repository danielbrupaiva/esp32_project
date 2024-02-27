#include "esp32-i2c.h"

#include <esp_log.h>
#include "mpu6050.h"

static const char *TAG = "MPU6050";

extern uint32_t acquisition_time_ms;

volatile mpu6050_acceleration_float_t acceleration;

volatile mpu6050_rotation_float_t gyroscope;

void configure_i2c(void)
{
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
}
void configure_mpu6050(void)
{
    configure_i2c();
//    mpu6050_reset();
//    ESP_LOGI(TAG, "reset successfully");
    mpu6050_init();

    while (!mpu6050_test_connection()) {}

    if (mpu6050_test_connection()) {
        ESP_LOGI(TAG, "mpu detected");
    }
}
mpu6050_acceleration_float_t get_mpu6050_accel_data(void)
{
    mpu6050_acceleration_t raw_acc_data = {0};
    float acc_scale_factor = 16384.0;
    mpu6050_get_acceleration(&raw_acc_data);

    acceleration.x = (float) raw_acc_data.x / acc_scale_factor;
    acceleration.y = (float) raw_acc_data.y / acc_scale_factor;
    acceleration.z = (float) raw_acc_data.z / acc_scale_factor;
    ESP_LOGI(TAG, "accel_x: %0.2f | accel_y: %0.2f | accel_z: %0.2f",
             (float) acceleration.x, (float) acceleration.y, (float) acceleration.z);
    return acceleration;
}

mpu6050_rotation_float_t get_mpu6050_gyro_data(void)
{
    mpu6050_rotation_t raw_gyro_data = {0};
    float gyro_scale_factor = 131.0;
    mpu6050_get_rotation(&raw_gyro_data);

    gyroscope.x = (float) raw_gyro_data.x / gyro_scale_factor;
    gyroscope.y = (float) raw_gyro_data.y / gyro_scale_factor;
    gyroscope.z = (float) raw_gyro_data.z / gyro_scale_factor;
    ESP_LOGI(TAG, "gyro_x: %0.2f | gyro_y: %0.2f | gyro_z: %0.2f",
             (float) gyroscope.x, (float) gyroscope.y, (float) gyroscope.z);
    return gyroscope;
}
void xMPU6050(void *arg)
{
    configure_mpu6050();
    for (;;) {
        get_mpu6050_accel_data();
        get_mpu6050_gyro_data();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}