#pragma once

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include <esp_http_server.h>

static httpd_handle_t start_webserver(void);
static void stop_webserver(httpd_handle_t server);
static void connect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
static void disconnect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
static esp_err_t get_root_handler(httpd_req_t *req);
static esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err);

static esp_err_t get_root_handler(httpd_req_t *req){
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

    esp_err_t error = httpd_resp_send(req, response, strlen(response) );

    if(ESP_OK != error){
        ESP_LOGI(TAG, "Erro %d while sending response", error);
        return error;
    }
    ESP_LOGI(TAG, "Response sent successfully");
    return error;
};

static httpd_handle_t start_webserver(void){
    static const char *TAG = "HTTP_SERVER";
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        static const httpd_uri_t root = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = get_root_handler,
            .user_ctx  = "ESP32 web server application example for control and monitor field inputs"
        };
        httpd_register_uri_handler(server, &root);
        return server;
    }
    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
};
static void stop_webserver(httpd_handle_t server){
    // Stop the httpd server
    httpd_stop(server);
};
static void connect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
    static const char *TAG = "HTTP_SERVER";
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (NULL == *server) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
};
static void disconnect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
    static const char *TAG = "HTTP_SERVER";
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
};
static esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err){
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "404 URI not found");
    return ESP_FAIL;
};