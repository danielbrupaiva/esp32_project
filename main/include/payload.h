#pragma once

#include "esp32-defines.h"

static volatile uint64_t payload_id = 0;

char *create_json_object(timestamp_t *timestamp, char *strftime_buf);

void xPayload(void *arg);