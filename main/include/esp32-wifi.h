#pragma once

#include "esp32-defines.h"
#include "esp32-http-server.h"

/* EventGroups */
static EventGroupHandle_t wifi_event_group;

#define WIFI_CONNECTED_BIT  BIT0
#define      WIFI_FAIL_BIT  BIT1

/* Wifi */
#define ESP_WIFI_CHANNEL CONFIG_ESP_WIFI_CHANNEL
#define ESP_MAX_STA_CONN CONFIG_ESP_MAX_STA_CONN

#define ESP_WIFI_SSID CONFIG_ESP_WIFI_SSID
#define ESP_WIFI_PASS CONFIG_ESP_WIFI_PASSWORD
#define ESP_MAXIMUM_RETRY CONFIG_ESP_MAXIMUM_RETRY

#if CONFIG_ESP_WIFI_AUTH_OPEN
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#elif CONFIG_ESP_WIFI_AUTH_WEP
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP
#elif CONFIG_ESP_WIFI_AUTH_WPA_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WAPI_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK
#endif

#ifdef __cplusplus
extern "C" {
#endif

static const wifi_config_t wifi_config = {
    .ap = {
        .ssid = ESP_WIFI_SSID,
        .ssid_len = strlen(ESP_WIFI_SSID),
        .channel = ESP_WIFI_CHANNEL,
        .password = ESP_WIFI_PASS,
        .max_connection = ESP_MAX_STA_CONN,
        .authmode = WIFI_AUTH_WPA_WPA2_PSK
    },
    .sta = {
        .ssid = ESP_WIFI_SSID,
        .password = ESP_WIFI_PASS,
        .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
        .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
    }
};

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