
idf_component_register(
        SRCS "main.c" "src/esp32-wifi.c" "src/esp32-http-server.c" "src/esp32_i2c.c" "src/esp32-i2c_rw.c" "src/mpu6050.c" "src/cJSON.c" "src/cJSON_Utils.c" "src/timestamp.c"
        INCLUDE_DIRS "."
        INCLUDE_DIRS "./include"
)