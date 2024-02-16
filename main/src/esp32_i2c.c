#include "esp32-i2c.h"


void configure_i2c(void)
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
}
void configure_mpu6050(void)
{
    static const char *TAG = "MPU6050";

    configure_i2c();
    //mpu6050_reset();
    //ESP_LOGI(TAG, "reset successfully");
    mpu6050_init();

    while (!mpu6050_test_connection()) {}

    if (mpu6050_test_connection()) {
        ESP_LOGI(TAG, "mpu detected");
    }
}
mpu6050_acceleration_t get_mpu6050_accel_data(void)
{
    static const char *TAG = "MPU6050";

    mpu6050_acceleration_t accel = {0};
    mpu6050_acceleration_t raw_acc_data = {0};
    float acc_scale_factor = 16384.0;
    mpu6050_get_acceleration(&raw_acc_data);

    accel.x = (float) raw_acc_data.x / acc_scale_factor;
    accel.y = (float) raw_acc_data.y / acc_scale_factor;
    accel.z = (float) raw_acc_data.z / acc_scale_factor;
    ESP_LOGI(TAG, "accel_x: %0.2f | accel_y: %0.2f | accel_z: %0.2f",
             (float) accel.x, (float) accel.y, (float) accel.z);
    return accel;
}

mpu6050_rotation_t get_mpu6050_gyro_data(void)
{
    static const char *TAG = "MPU6050";

    mpu6050_rotation_t gyro = {0};
    mpu6050_rotation_t raw_gyro_data = {0};
    float gyro_scale_factor = 131.0;
    mpu6050_get_rotation(&raw_gyro_data);

    gyro.x = (float) raw_gyro_data.x / gyro_scale_factor;
    gyro.y = (float) raw_gyro_data.y / gyro_scale_factor;
    gyro.z = (float) raw_gyro_data.z / gyro_scale_factor;
    ESP_LOGI(TAG, "gyro_x: %0.2f | gyro_y: %0.2f | gyro_z: %0.2f",
             (float) gyro.x, (float) gyro.y, (float) gyro.z);
    return gyro;
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