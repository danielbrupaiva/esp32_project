#pragma once
// Wifi
#include "esp_wifi.h"
// WiFi Callback
#include "esp_event.h"
// Lwip
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "lwip/api.h"
#include "lwip/netdb.h"
#include "lwip/ip4_addr.h"

// wifi
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

static const char *TAG = "xWifi";

static void configure_wifi(wifi_mode_t mode);
static void configure_wifi_station_mode(void);
static void configure_wifi_access_point_mode(void);
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

// EventGroups
static EventGroupHandle_t wifi_event_group;

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1


static int s_retry_num = 0;

/** Configure and start wifi connection **/
void configure_wifi(wifi_mode_t mode)
{
    ESP_LOGI(TAG, "configure_wifi");
    /** Create event group to sync task **/
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    switch (mode) {
        case WIFI_MODE_NULL:ESP_LOGI(TAG, "WIFI_MODE_NULL");
            break;
        case WIFI_MODE_STA:ESP_LOGI(TAG, "WIFI_MODE_STA");
            configure_wifi_station_mode();
            break;
        case WIFI_MODE_AP:ESP_LOGI(TAG, "WIFI_MODE_AP");
            configure_wifi_access_point_mode();
            break;
        case WIFI_MODE_APSTA:ESP_LOGI(TAG, "WIFI_MODE_APSTA");
            break;
        case WIFI_MODE_MAX:ESP_LOGI(TAG, "WIFI_MODE_MAX");
            break;
        default:ESP_LOGI(TAG, "default");
            break;
    }
    //vEventGroupDelete(wifi_event_group);
}

void configure_wifi_access_point_mode(void)
{
    ESP_LOGI(TAG, "configure_wifi_access_point_mode");
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_AP_STAIPASSIGNED,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = ESP_WIFI_SSID,
            .ssid_len = strlen(ESP_WIFI_SSID),
            .channel = ESP_WIFI_CHANNEL,
            .password = ESP_WIFI_PASS,
            .max_connection = ESP_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK},
    };
    if (strlen(ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "configure_wifi_access_point_mode finished. SSID:%s password: channel:%d",
             ESP_WIFI_SSID, ESP_WIFI_CHANNEL);

    EventBits_t bits = xEventGroupWaitBits(wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", ESP_WIFI_SSID, ESP_WIFI_PASS);
    }
    else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s", ESP_WIFI_SSID, ESP_WIFI_PASS);
    }
    else {
        ESP_LOGE(TAG, "TIMEOUT! NO CONNECTION ESTABLISH");
    }
}

void configure_wifi_station_mode(void)
{
    ESP_LOGI(TAG, "configure_wifi_station_mode");
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = ESP_WIFI_SSID,
            .password = ESP_WIFI_PASS,
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
            .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", ESP_WIFI_SSID, ESP_WIFI_PASS);
    }
    else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s", ESP_WIFI_SSID, ESP_WIFI_PASS);
    }
    else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    /* The event will not be processed after unregister */
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
}

void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG, "wifi_event_handler");

    if (event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    }
    else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else {
            xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    }
    else if (event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
        /** Access point mode events handlers**/
    else if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *) event_data;
        ESP_LOGI(TAG, "station " MACSTR " join, AID=%d", MAC2STR(event->mac), event->aid);
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *) event_data;
        ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d", MAC2STR(event->mac), event->aid);
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void _wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id) {
        case WIFI_EVENT_WIFI_READY:               /* ESP32 WiFi ready */
            break;
        case WIFI_EVENT_SCAN_DONE:                /* ESP32 finish scanning AP */
            break;
        case WIFI_EVENT_STA_START:                /* ESP32 station start */
            break;
        case WIFI_EVENT_STA_STOP:                 /* ESP32 station stop */
            break;
        case WIFI_EVENT_STA_CONNECTED:            /* ESP32 station connected to AP */
            break;
        case WIFI_EVENT_STA_DISCONNECTED:         /* ESP32 station disconnected from AP */
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
            break;
        case WIFI_EVENT_AP_STADISCONNECTED:       /* a station disconnected from ESP32 soft-AP */
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

void ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id) {
        case IP_EVENT_STA_GOT_IP:               /* station got IP from connected AP */
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