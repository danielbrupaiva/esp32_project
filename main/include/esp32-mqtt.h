#pragma once

#include <esp_log.h>
#include <esp_system.h>
#include <esp_event.h>
#include <esp_event_loop.h>
#include <freertos/event_groups.h>

#include "esp32-defines.h"

#define DEBUG true

#define MQTT_ADDR "broker.emqx.io"
#define MQTT_USER ""
#define MQTT_PASS MQTT_USER
// Lib MQTT
#include "mqtt_client.h"

#ifdef __cplusplus
extern "C" {
#endif

static EventGroupHandle_t mqtt_event_group;

esp_mqtt_client_handle_t mqtt_client;

static esp_err_t mqtt_event_handler_callback(esp_mqtt_event_handle_t event);

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
static void mqtt_app_start(void);
static void mqtt_publish_msg(char *msg);
void xMQTTPublish(void *arg)
{
    mqtt_app_start();
    static const char *TAG = "MQTT PUBLISH ";
    for (;;) {
        mqtt_publish_msg("payload");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

// MQTT
static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    static const char *TAG = "MQTT_HANDLER_CB";

    mqtt_client = event->client;
    int8_t msg_id;

    switch (event->event_id) {
        /*
         * Este case foi adicionado a partir da versão 3.2 do SDK-IDF;
         * e é necessário ser adicionado devido as configurações de error do compilador;
         */
        case MQTT_EVENT_BEFORE_CONNECT:

            if (DEBUG)
                ESP_LOGI(TAG, "MQTT_EVENT_BEFORE_CONNECT");

            break;

            /**
             * Evento chamado quando o ESP32 se conecta ao broker MQTT, ou seja,
             * caso a conexão socket TCP, SSL/TSL e autenticação com o broker MQTT
             * tenha ocorrido com sucesso, este evento será chamado informando que
             * o ESP32 está conectado ao broker;
             */
        case MQTT_EVENT_CONNECTED:

            if (DEBUG) {
                ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
                msg_id = esp_mqtt_client_subscribe(mqtt_client, "node", 0);
                ESP_LOGI(TAG, "SUBSCRIBED TO TOPIC:%d", msg_id);
            }

            /**
             * Se chegou aqui é porque o ESP32 está conectado ao Broker MQTT;
             * A notificação é feita setando em nível 1 o bit WIFI_CONNECTED_BIT da
             * variável mqtt_event_group;
             */
            xEventGroupSetBits(mqtt_event_group, WIFI_CONNECTED_BIT);
            break;
            /**
             * Evento chamado quando o ESP32 for desconectado do broker MQTT;
             */
        case MQTT_EVENT_DISCONNECTED:

            if (DEBUG)
                ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            /**
             * Se chegou aqui é porque o ESP32 foi desconectado do broker MQTT;
             */
            xEventGroupClearBits(mqtt_event_group, WIFI_CONNECTED_BIT);
            break;

            /**
             * O eventos seguintes são utilizados para notificar quando um tópico é
             * assinado pelo ESP32;
             */
        case MQTT_EVENT_SUBSCRIBED:ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED TOPIC:%d", event->msg_id);
            msg_id = esp_mqtt_client_publish(mqtt_client, "node", "data", 0, 0, 0);
            break;

            /**
             * Quando a assinatura de um tópico é cancelada pelo ESP32,
             * este evento será chamado;
             */
        case MQTT_EVENT_UNSUBSCRIBED:ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;

            /**
             * Este evento será chamado quando um tópico for publicado pelo ESP32;
             */
        case MQTT_EVENT_PUBLISHED:

            if (DEBUG)
                ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;

            /**
             * Este evento será chamado quando uma mensagem chegar em algum tópico
             * assinado pelo ESP32;
             */
        case MQTT_EVENT_DATA:

            if (DEBUG) {
                ESP_LOGI(TAG, "MENSSAGE ARRIVED");
                /**
                 * Sabendo o nome do tópico que publicou a mensagem é possível
                 * saber a quem data pertence;
                 */
                ESP_LOGI(TAG, "TOPIC=%.*s", event->topic_len, event->topic);
                ESP_LOGI(TAG, "DATA=%.*s", event->data_len, event->data);
            }
            break;

            /**
             * Evento chamado quando ocorrer algum erro na troca de informação
             * entre o ESP32 e o Broker MQTT;
             */
        case MQTT_EVENT_ERROR:
            if (DEBUG)
                ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;

        case MQTT_EVENT_ANY:ESP_LOGI(TAG, "MQTT_EVENT_ANY");
            break;
        case MQTT_EVENT_DELETED:break;
    }
    return ESP_OK;
}
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    static const char *TAG = "MQTT";
    if (DEBUG)
        ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

static void mqtt_app_start(void)
{

    const esp_mqtt_client_config_t mqtt_cfg =
        {
            .uri = MQTT_ADDR,
            .username = MQTT_USER,
            .password = MQTT_PASS,
            //.cert_pem = (const char *)mqtt_eclipse_org_pem_start,
        };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}

static void mqtt_publish_msg(char *msg)
{
    static const char *TAG = "MQTT";
    static uint64_t id = 0;

}
#ifdef __cplusplus
}
#endif
