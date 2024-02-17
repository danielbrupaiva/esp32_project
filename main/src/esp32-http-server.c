#include "esp32-http-server.h"
#include "payload.h"
#include "timestamp.h"
/* HTTP server */
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
// The asynchronous response
static void generate_async_resp(void *arg)
{
    static const char *TAG = "HTTP_SERVER";
    // Data format to be sent from the server as a response to the client
    char http_string[250];

    timestamp_t timestamp = get_timestamp();
    char strftime_buf[120];
    strftime(strftime_buf, sizeof(strftime_buf), "%Y-%m-%d %H:%M:%S", &timestamp.timeinfo);

    char *data_string = create_json_object(&timestamp, strftime_buf);
//    char *data_string = "Hello from ESP32 websocket server ...";
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

httpd_handle_t http_server_start(void)
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

