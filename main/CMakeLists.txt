
idf_component_register(
        SRCS "main.c" "./src/esp32-i2c_rw.c" "./src/mpu6050.c" "./src/cJSON.c" "./src/cJSON_Utils.c"
        INCLUDE_DIRS "."
        INCLUDE_DIRS "./include"
)