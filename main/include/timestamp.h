#pragma once

#include "esp32-defines.h"
#ifdef __cplusplus
extern "C" {
#endif
void time_sync_notification_cb(struct timeval *tv);
struct tm get_time_info(void);

void initialize_sntp(void);

void print_timestamp();

timestamp_t get_timestamp();

void xTimeStamp(void *arg);

#ifdef __cplusplus
}
#endif
