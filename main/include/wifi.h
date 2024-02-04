#pragma once

#define WIFI_SSID "BRUG.2G"
#define WIFI_PASS "*admin1101"
//#define WIFI_SSID "Pos_Sist_Emb"
//#define WIFI_PASS "LPFS2022"

#define SERVER_IP "192.168.15.23"
#define SERVER_PORT 80


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
// EventGroups
static EventGroupHandle_t wifi_event_group;
static int WIFI_CONNECTED_BIT = BIT0;
static int MQTT_CONNECTED_BIT = BIT1;

static void wifi_init_start(void);
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
void configure_start_wifi(void);

/** initialization **/
static void wifi_init_start(void)
{
    static const char *TAG = "xWifi : Initializing";
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
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
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    if (DEBUG)
    {
        ESP_LOGI(TAG, "wifi_init_start finished.");
        ESP_LOGI(TAG, "connect to ap SSID:%s password",
                 WIFI_SSID);
    }
}
/** WIFI event handler **/
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    static const char *TAG = "xWifi : Event Handler";
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        if (DEBUG)
        {
            ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
            ESP_LOGI(TAG, "got ip: " IPSTR, IP2STR(&event->ip_info.ip));
        }
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}
/** Configure and start wifi connection **/
void configure_start_wifi(void)
{
    /** Create event group to sync task **/
    wifi_event_group = xEventGroupCreate();
    /** Start Wfi initialization **/
    wifi_init_start();
    /** Wait connection stablish**/
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, true, true, 10000 / portTICK_PERIOD_MS);
}

/**
 * 1-Configura o ESP32 em modo Socket Client;
 * 2-Conecta ao servidor TCP;
 * 3-Envia string para o servidor;
 * 4-Encerra a conexão socket com o servidor;
 */
static void socket_client(int count)
{
    static const char *TAG = "xWifi : socket_client";

	int rc;
	char str[20];

	if (DEBUG)
		ESP_LOGI(TAG, "socket_client...\n");

	/**
	 * Cria o socket TCP;
	 */
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (DEBUG)
		ESP_LOGI(TAG, "socket: rc: %d", sock);

	/**
	 * Configura/Vincula o endereço IP e Port do Servidor (Bind);
	 */
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	inet_pton(AF_INET, SERVER_IP, &serverAddress.sin_addr.s_addr);
	serverAddress.sin_port = htons(SERVER_PORT);

	/**
	 * Tenta estabelecer a conexão socket TCP entre ESP32 e Servidor;
	 */
	rc = connect(sock, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr_in));

	/**
	 * error? (-1)
	 */
	if (DEBUG)
		ESP_LOGI(TAG, "connect rc: %d", rc);

	if (rc == 0)
	{
		/**
		 * Converte o valor de count para string e envia a string
		 * ao servidor via socket tcp;
		 */
		sprintf(str, "Count = %d\r\n", count);
		rc = send(sock, str, strlen(str), 0);
		/**
		 * error durante o envio? (-1)
		 */
		if (DEBUG)
			ESP_LOGI(TAG, "send: rc: %d", rc);
	}

	rc = close(sock);
	/**
	 * error no fechamento do socket? (-1)
	 */
	if (DEBUG)
		ESP_LOGI(TAG, "close: rc: %d", rc);
}