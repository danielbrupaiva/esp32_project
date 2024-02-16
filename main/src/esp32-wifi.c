#include "esp32-wifi.h"

/* Wifi */
/** Configure and start wifi connection **/
void wifi_configure(wifi_mode_t wifi_mode, wifi_config_t *config)
{
    static const char *TAG = "Wifi";
    ESP_LOGI(TAG, "wifi_configure");
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT,
                                               ESP_EVENT_ANY_ID,
                                               &wifi_event_handler,
                                               NULL));

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT,
                                               ESP_EVENT_ANY_ID,
                                               &wifi_ip_event_handler,
                                               NULL));

    switch (wifi_mode) {
        case WIFI_MODE_NULL:ESP_LOGI(TAG, "WIFI_MODE_NULL");
            break;
        case WIFI_MODE_STA:ESP_LOGI(TAG, "WIFI_MODE_STA");
            wifi_station_mode_configure((wifi_config_t *) &config->sta);
            break;
        case WIFI_MODE_AP:ESP_LOGI(TAG, "WIFI_MODE_AP");
            wifi_access_point_mode_configure((wifi_config_t *) &config->ap);
            break;
        case WIFI_MODE_APSTA:ESP_LOGI(TAG, "WIFI_MODE_APSTA");
            break;
        case WIFI_MODE_MAX:ESP_LOGI(TAG, "WIFI_MODE_MAX");
            break;
        default:ESP_LOGI(TAG, "default");
            break;
    }
    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
    * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s", ESP_WIFI_SSID);
    }
    else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s", ESP_WIFI_SSID);
    }
    else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
    vEventGroupDelete(wifi_event_group);
}

void wifi_access_point_mode_configure(wifi_config_t *config)
{
    static const char *TAG = "Wifi";
    ESP_LOGI(TAG, "wifi_access_point_mode_configure");
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    if (strlen(ESP_WIFI_PASS) == 0) {
        config->ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "wifi_access_point_mode_configure finished. SSID:%s", ESP_WIFI_SSID);
}

void wifi_station_mode_configure(wifi_config_t *config)
{
    static const char *TAG = "Wifi";
    ESP_LOGI(TAG, "wifi_station_mode_configure");
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "wifi_station_mode_configure finished. SSID:%s", ESP_WIFI_SSID);
}

void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    static const char *TAG = "Wifi";
    ESP_LOGI(TAG, "wifi_event_handler");

    switch (event_id) {
        case WIFI_EVENT_WIFI_READY:               /* ESP32 WiFi ready */
            break;
        case WIFI_EVENT_SCAN_DONE:                /* ESP32 finish scanning AP */
            break;
        case WIFI_EVENT_STA_START:                /* ESP32 station start */
            esp_wifi_connect();
            break;
        case WIFI_EVENT_STA_STOP:                 /* ESP32 station stop */
            break;
        case WIFI_EVENT_STA_CONNECTED:            /* ESP32 station connected to AP */
            http_server_start();
            break;
        case WIFI_EVENT_STA_DISCONNECTED:         /* ESP32 station disconnected from AP */
            if (s_connection_retries < ESP_MAXIMUM_RETRY) {
                esp_wifi_connect();
                s_connection_retries++;
                ESP_LOGI(TAG, "retry to connect to the AP");
            }
            else {
                xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
            }
            ESP_LOGI(TAG, "connect to the AP fail");
            break;
        case WIFI_EVENT_STA_AUTHMODE_CHANGE:      /* the auth mode of AP connected by ESP32 station changed */
            break;
        case WIFI_EVENT_STA_WPS_ER_SUCCESS:       /* ESP32 station wps succeeds in enrollee mode */
            break;
        case WIFI_EVENT_STA_WPS_ER_FAILED:        /* ESP32 station wps fails in enrollee mode */
            break;
        case WIFI_EVENT_STA_WPS_ER_TIMEOUT:       /* ESP32 station wps timeout in enrollee mode */
            break;
        case WIFI_EVENT_STA_WPS_ER_PIN:           /* ESP32 station wps pin code in enrollee mode */
            break;
        case WIFI_EVENT_STA_WPS_ER_PBC_OVERLAP:   /* ESP32 station wps overlap in enrollee mode */
            break;
        case WIFI_EVENT_AP_START:                 /* ESP32 soft-AP start */
            break;
        case WIFI_EVENT_AP_STOP:                  /* ESP32 soft-AP stop */
            break;
        case WIFI_EVENT_AP_STACONNECTED:          /* a station connected to ESP32 soft-AP */
            xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
            break;
        case WIFI_EVENT_AP_STADISCONNECTED:       /* a station disconnected from ESP32 soft-AP */
            xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
            break;
        case WIFI_EVENT_AP_PROBEREQRECVED:        /* Receive probe request packet in soft-AP interface */
            break;
        case WIFI_EVENT_FTM_REPORT:               /* Receive report of FTM procedure */
            break;
        case WIFI_EVENT_STA_BSS_RSSI_LOW:         /* AP's RSSI crossed configured threshold */
            break;
        case WIFI_EVENT_ACTION_TX_STATUS:         /* Status indication of Action Tx operation */
            break;
        case WIFI_EVENT_ROC_DONE:                 /* Remain-on-Channel operation complete */
            break;
        case WIFI_EVENT_STA_BEACON_TIMEOUT:       /* ESP32 station beacon timeout */
            break;
        case WIFI_EVENT_MAX:                      /* Invalid WiFi event ID */
            break;
    }
}

void wifi_ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    static const char *TAG = "Wifi";
    ESP_LOGI(TAG, "wifi_ip_event_handler");
    switch (event_id) {
        case IP_EVENT_STA_GOT_IP:               /* station got IP from connected AP */
            s_connection_retries = 0;
            xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
            break;
        case IP_EVENT_STA_LOST_IP:              /* station lost IP and the IP is reset to 0 */
            break;
        case IP_EVENT_AP_STAIPASSIGNED:         /* soft-AP assign an IP to a connected station */
            break;
        case IP_EVENT_GOT_IP6:                  /* station or ap or ethernet interface v6IP addr is preferred */
            break;
        case IP_EVENT_ETH_GOT_IP:               /* ethernet got IP from connected AP */
            break;
        case IP_EVENT_ETH_LOST_IP:              /* ethernet lost IP and the IP is reset to 0 */
            break;
        case IP_EVENT_PPP_GOT_IP:               /* PPP interface got IP */
            break;
        case IP_EVENT_PPP_LOST_IP:              /* PPP interface lost IP */
            break;
    }
}

