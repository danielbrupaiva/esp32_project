#pragma once

#include "esp32-defines.h"
#include "timestamp.h"

#ifdef __cplusplus
extern "C" {
#endif

static uint64_t payload_id = 0;

static char *json_buffer;

char *create_json_object(timestamp_t *timestamp, char *strftime_buf);

void xPayload(void *arg);

#ifdef __cplusplus
}
#endif