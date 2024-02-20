#pragma once

#include "esp32-defines.h"
#include "esp32-http-server.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Numbers of retries STA mode */
static int s_connection_retries = 0;

/* Wifi */
void wifi_configure(wifi_mode_t wifi_mode, wifi_config_t *config);
void wifi_station_mode_configure(wifi_config_t *config);
void wifi_access_point_mode_configure(wifi_config_t *config);
void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

void wifi_ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
#ifdef __cplusplus
}
#endif