
idf_component_register(
        SRCS "main.c"
        "src/esp32-wifi.c"
        "src/esp32-http-server.c"
        "src/esp32-gpio.c"
        "src/esp32-adc.c"
        "src/esp32_i2c.c"
        "src/esp32-i2c_rw.c"
        "src/mpu6050.c"
        "src/timestamp.c"
        "src/payload.c"
        "src/cJSON.c"
        "src/cJSON_Utils.c"
        INCLUDE_DIRS "."
        INCLUDE_DIRS "./include"
)

spiffs_create_partition_image(storage ../data FLASH_IN_PROJECT)