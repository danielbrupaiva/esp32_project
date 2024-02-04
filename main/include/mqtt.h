#pragma once

#define MQTT_ADDR "mqtt://localhost:1883"
#define MQTT_USER "admin"
#define MQTT_PASS MQTT_USER
// Lib MQTT
#include "mqtt_client.h"

static EventGroupHandle_t mqtt_event_group;

esp_mqtt_client_handle_t mqtt_client;
static esp_err_t mqtt_event_handler_callback(esp_mqtt_event_handle_t event);
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
static void mqtt_app_start(void);
static void mqtt_publish_msg(void);

static void xMQTTPublish(void *arg)
{
    static const char *TAG = "MQTT PUBLISH: ";
    for (;;)
    {

    }
}
// MQTT
static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    static const char *TAG = "MQTT_HANDLER_CB";

    mqtt_client = event->client;
    int8_t msg_id;

    switch (event->event_id)
    {
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

        if (DEBUG)
        {
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
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED TOPIC:%d", event->msg_id);
        msg_id = esp_mqtt_client_publish(mqtt_client, "node", "data", 0, 0, 0);
        break;

    /**
     * Quando a assinatura de um tópico é cancelada pelo ESP32,
     * este evento será chamado;
     */
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
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

        if (DEBUG)
        {
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

    case MQTT_EVENT_ANY:
        ESP_LOGI(TAG, "MQTT_EVENT_ANY");
        break;
    case MQTT_EVENT_DELETED:
        break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
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
    mqtt_publish_msg();
}
static void mqtt_publish_msg(void)
{
    id++;
    cJSON *data;
    /* Cria a estrutura de dados MONITOR a ser enviado por JSON */
    data = cJSON_CreateObject();
    cJSON_AddNumberToObject(data, "ID", id);
    cJSON_AddBoolToObject(data, "btnEmerg", btnEmerg);
    cJSON_AddBoolToObject(data, "btnStart", btnStart);
    cJSON_AddStringToObject(data, "status", status);
    cJSON_AddNumberToObject(data, "sensor", sensor);

    if (esp_mqtt_client_publish(mqtt_client, "data", cJSON_Print(data), strlen(cJSON_Print(data)), 0, 0) == 0)
    {
        if (DEBUG)
        {
            ESP_LOGI(TAG, "Info data:%s", cJSON_Print(data));
            ESP_LOGI(TAG, "Mensagem publicada com sucesso!");
        }
    }
    cJSON_Delete(data);

    cJSON *mpu6050_data;
    mpu6050_data = cJSON_CreateObject();

    cJSON_AddNumberToObject(mpu6050_data, "ID", id);
    cJSON_AddNumberToObject(mpu6050_data, "accel_x", accel_x);
    cJSON_AddNumberToObject(mpu6050_data, "accel_y", accel_y);
    cJSON_AddNumberToObject(mpu6050_data, "accel_z", accel_z);
    cJSON_AddNumberToObject(mpu6050_data, "gyro_x", accel_x);
    cJSON_AddNumberToObject(mpu6050_data, "gyro_y", accel_y);
    cJSON_AddNumberToObject(mpu6050_data, "gyro_z", accel_z);

    if (esp_mqtt_client_publish(mqtt_client, "mpu6050", cJSON_Print(mpu6050_data), strlen(cJSON_Print(mpu6050_data)), 0, 0) == 0)
    {
        if (DEBUG)
        {
            ESP_LOGI(TAG, "Info mpu6050_data:%s", cJSON_Print(mpu6050_data));
            ESP_LOGI(TAG, "Mensagem publicada com sucesso!");
        }
    }
    cJSON_Delete(mpu6050_data);
    /*
        { //TAGO IOT
          data = cJSON_CreateObject();

          cJSON_AddBoolToObject(data, "btnEmerg", btnEmerg);

          if (esp_mqtt_client_publish(mqtt_client, "data", cJSON_Print(data), strlen(cJSON_Print(data)), 0, 0) == 0)
          {
              if (DEBUG)
              {
                  ESP_LOGI(TAG, "Info data:%s", cJSON_Print(data));
                  ESP_LOGI(TAG, "Mensagem publicada com sucesso!");
              }
          }
          cJSON_Delete(data);

          data = cJSON_CreateObject();
          cJSON_AddBoolToObject(data, "btnStart", btnStart);

          if (esp_mqtt_client_publish(mqtt_client, "data", cJSON_Print(data), strlen(cJSON_Print(data)), 0, 0) == 0)
          {
              if (DEBUG)
              {
                  ESP_LOGI(TAG, "Info data:%s", cJSON_Print(data));
                  ESP_LOGI(TAG, "Mensagem publicada com sucesso!");
              }
          }
          cJSON_Delete(data);
          data = cJSON_CreateObject();
          cJSON_AddStringToObject(data, "status", status);

          if (esp_mqtt_client_publish(mqtt_client, "data", cJSON_Print(data), strlen(cJSON_Print(data)), 0, 0) == 0)
          {
              if (DEBUG)
              {
                  ESP_LOGI(TAG, "Info data:%s", cJSON_Print(data));
                  ESP_LOGI(TAG, "Mensagem publicada com sucesso!");
              }
          }
          cJSON_Delete(data);

          data = cJSON_CreateObject();
          cJSON_AddNumberToObject(data, "sensor", sensor);

          if (esp_mqtt_client_publish(mqtt_client, "data", cJSON_Print(data), strlen(cJSON_Print(data)), 0, 0) == 0)
          {
              if (DEBUG)
              {
                  ESP_LOGI(TAG, "Info data:%s", cJSON_Print(data));
                  ESP_LOGI(TAG, "Mensagem publicada com sucesso!");
              }
          }
          cJSON_Delete(data);
        }*/
}
