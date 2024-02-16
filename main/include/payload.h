#pragma once

#include <esp_log.h>
#include "cJSON.h"
#include "cJSON_Utils.h"
#include "mpu6050.h"


typedef struct _payload
{
    uint32_t id;
    mpu6050_acceleration_t accel;
    mpu6050_rotation_t gyro;

} payload_t;

static uint64_t id = 0;

char *create_json_object(void);
