#pragma once
#include <esp_log.h>

#include <esp_system.h>
#include <esp_event.h>
#include "esp_event_loop.h"#include <sys/param.h>
/* Wifi */
#include <esp_wifi.h>
#include <esp_netif.h>
#include <esp_netif_types.h>
#include <esp_wifi_types.h>
#include <esp_http_server.h>
/* Lwip */
#include <lwip/err.h>
#include <lwip/sys.h>
#include <lwip/sockets.h>
#include <lwip/api.h>
#include <lwip/netdb.h>
#include <lwip/ip4_addr.h>
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


/* Wifi */
static void wifi_configure(wifi_mode_t wifi_mode, wifi_config_t *config);
static void wifi_station_mode_configure(wifi_config_t *config);
static void wifi_access_point_mode_configure(wifi_config_t *config);
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void wifi_ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void _wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
/* HTTP server */
static httpd_handle_t http_server_start(void);
static void http_server_stop(httpd_handle_t server);
static void http_server_connect_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void http_server_disconnect_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static esp_err_t http_server_get_root_handler(httpd_req_t *req);
static esp_err_t http_server_async_get_handler(httpd_req_t *req);
static esp_err_t http_server_404_error_handler(httpd_req_t *req, httpd_err_code_t err);

/* EventGroups */
static EventGroupHandle_t wifi_event_group;

/* Numbers of retries STA mode */
static int s_retry_num = 0;

#define WIFI_CONNECTED_BIT  BIT0
#define      WIFI_FAIL_BIT  BIT1

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
        ESP_LOGI(TAG, "connected to ap SSID:%s password:", ESP_WIFI_SSID);
    }
    else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:", ESP_WIFI_SSID);
    }
    else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
    //vEventGroupDelete(wifi_event_group);
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
            if (s_retry_num < ESP_MAXIMUM_RETRY) {
                esp_wifi_connect();
                s_retry_num++;
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
            s_retry_num = 0;
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

static esp_err_t http_server_get_root_handler(httpd_req_t *req)
{
    static const char *TAG = "HTTP_SERVER";
    const char *response = "<!DOCTYPE html>\n"
                           "<html lang=\"en\">\n"
                           "<head>\n"
                           "    <meta charset=\"UTF-8\">\n"
                           "    <title>ESP32 WebServer</title>\n"
                           "    <style>\n"
                           "    </style>\n"
                           "</head>\n"
                           "<body>\n"
                           "<h1>ESP32 WebServer application example</h1>\n"
                           "</body>\n"
                           "</html>";

    esp_err_t error = httpd_resp_send(req, response, strlen(response));

    if (ESP_OK != error) {
        ESP_LOGI(TAG, "Erro %d while sending response", error);
        return error;
    }
    ESP_LOGI(TAG, "Response sent successfully");
    return error;
};
// Asynchronous response data structure
struct async_resp_arg
{
    httpd_handle_t hd; // Server instance
    int fd;            // Session socket file descriptor
};
// The asynchronous response
static void generate_async_resp(void *arg)
{
    static const char *TAG = "HTTP_SERVER";
    // Data format to be sent from the server as a response to the client
    char http_string[250];
    char *data_string = "Hello from ESP32 websocket server ...";
    sprintf(http_string, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n", strlen(data_string));

    // Initialize asynchronous response data structure
    struct async_resp_arg *resp_arg = (struct async_resp_arg *) arg;
    httpd_handle_t hd = resp_arg->hd;
    int fd = resp_arg->fd;

    // Send data to the client
    ESP_LOGI(TAG, "Executing queued work fd : %d", fd);
    httpd_socket_send(hd, fd, http_string, strlen(http_string), 0);
    httpd_socket_send(hd, fd, data_string, strlen(data_string), 0);

    free(arg);
}
static esp_err_t http_server_async_get_handler(httpd_req_t *req)
{
    static const char *TAG = "HTTP_SERVER";
    struct async_resp_arg *resp_arg = malloc(sizeof(struct async_resp_arg));
    resp_arg->hd = req->handle;
    resp_arg->fd = httpd_req_to_sockfd(req);
    ESP_LOGI(TAG, "Queuing work fd : %d", resp_arg->fd);
    httpd_queue_work(req->handle, generate_async_resp, resp_arg);
    return ESP_OK;
}

static httpd_handle_t http_server_start(void)
{
    static const char *TAG = "HTTP_SERVER";
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        static const httpd_uri_t uri_root = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = http_server_get_root_handler,
            .user_ctx  = "ESP32 web server application example for control and monitor field inputs"
        };
        static const httpd_uri_t uri_ws = {
            .uri = "/ws",           // URL added to WiFi's default gateway
            .method = HTTP_GET,
            .handler = http_server_async_get_handler,
            .user_ctx = "ESP32 WS",
        };
        httpd_register_uri_handler(server, &uri_root);
        httpd_register_uri_handler(server, &uri_ws);
        return server;
    }
    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
};

static void http_server_stop(httpd_handle_t server)
{
    static const char *TAG = "HTTP_SERVER";
    // Stop the httpd server
    httpd_stop(server);
    ESP_LOGI(TAG, "Server stop!");
};

static void http_server_connect_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    static const char *TAG = "HTTP_SERVER";
    httpd_handle_t *server = (httpd_handle_t *) arg;
    if (NULL == *server) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = http_server_start();
    }
};

static void http_server_disconnect_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    static const char *TAG = "HTTP_SERVER";
    httpd_handle_t *server = (httpd_handle_t *) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        http_server_stop(*server);
        *server = NULL;
    }
};

static esp_err_t http_server_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    static const char *TAG = "HTTP_SERVER";
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "404 URI not found");
    ESP_LOGI(TAG, "404 URI not found");
    return ESP_FAIL;
};