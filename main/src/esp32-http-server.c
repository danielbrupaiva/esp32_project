#include "esp32-http-server.h"
#include "payload.h"
#include "timestamp.h"
#include "esp32-gpio.h"
#define MIN(a, b) ((a) < (b) ? (a) : (b))

static char index_html_buffer[20480];

/* HTTP server */
static const char *TAG = "HTTP_SERVER";

static esp_err_t http_server_get_root_handler(httpd_req_t *req)
{
//    const char *response = index_html_buffer;

    esp_err_t error = httpd_resp_send(req, index_html_buffer, strlen(index_html_buffer));

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
    // Data format to be sent from the server as a response to the client
    char http_string[250];

    timestamp_t timestamp = get_timestamp();
    char strftime_buf[120];
    strftime(strftime_buf, sizeof(strftime_buf), "%Y-%m-%d %H:%M:%S", &timestamp.timeinfo);

    char *data_string = create_json_object(&timestamp, strftime_buf);
    sprintf(http_string, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n", strlen(data_string));

    // Initialize asynchronous response data structure
    struct async_resp_arg *resp_arg = (struct async_resp_arg *) arg;
    httpd_handle_t hd = resp_arg->hd;
    int fd = resp_arg->fd;

    // Send data to the client
    ESP_LOGI(TAG, "Executing queued work fd : %d", fd);
    httpd_socket_send(hd, fd, http_string, strlen(http_string), 0);
    httpd_socket_send(hd, fd, data_string, strlen(data_string), 0);
    free(data_string);
    free(arg);
}

static esp_err_t http_server_async_get_handler(httpd_req_t *req)
{
    struct async_resp_arg *resp_arg = malloc(sizeof(struct async_resp_arg));
    resp_arg->hd = req->handle;
    resp_arg->fd = httpd_req_to_sockfd(req);
    ESP_LOGI(TAG, "Queuing work fd : %d", resp_arg->fd);
    httpd_queue_work(req->handle, generate_async_resp, resp_arg);
    return ESP_OK;
}

static esp_err_t http_server_async_post_handler(httpd_req_t *req)
{
    char content[100];

    size_t recv_size = MIN(req->content_len, sizeof(content));

    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0) {  /* 0 return value indicates connection closed */
        /* Check if timeout occurred */
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            /* In case of timeout one can choose to retry calling
             * httpd_req_recv(), but to keep it simple, here we
             * respond with an HTTP 408 (Request Timeout) error */
            httpd_resp_send_408(req);
        }
        /* In case of error, returning ESP_FAIL will
         * ensure that the underlying socket is closed */
        return ESP_FAIL;
    }

    /* Send a simple response */
    const char resp[] = "URI POST Response";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static void ws_async_send(void *arg)
{
    httpd_ws_frame_t ws_pkt;
    struct async_resp_arg *resp_arg = arg;
    httpd_handle_t hd = resp_arg->hd;
    int fd = resp_arg->fd;

//  Get JSON to return client
    timestamp_t timestamp = get_timestamp();
    char strftime_buf[120];
    strftime(strftime_buf, sizeof(strftime_buf), "%Y-%m-%d %H:%M:%S", &timestamp.timeinfo);
    char *data_string = create_json_object(&timestamp, strftime_buf);

    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = (uint8_t *) data_string;
    ws_pkt.len = strlen(data_string);
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;

    static size_t max_clients = CONFIG_LWIP_MAX_LISTENING_TCP;
    size_t fds = max_clients;
    int client_fds[max_clients];

    esp_err_t ret = httpd_get_client_list(server, &fds, client_fds);

    if (ret != ESP_OK) {
        return;
    }

    for (int i = 0; i < fds; i++) {
        int client_info = httpd_ws_get_fd_info(server, client_fds[i]);
        if (client_info == HTTPD_WS_CLIENT_WEBSOCKET) {
            httpd_ws_send_frame_async(hd, client_fds[i], &ws_pkt);
        }
    }
    free(data_string);
    free(resp_arg);
}

static esp_err_t trigger_async_send(httpd_handle_t handle, httpd_req_t *req)
{
    struct async_resp_arg *resp_arg = malloc(sizeof(struct async_resp_arg));
    resp_arg->hd = req->handle;
    resp_arg->fd = httpd_req_to_sockfd(req);
    return httpd_queue_work(handle, ws_async_send, resp_arg);
}

static esp_err_t handle_ws_req(httpd_req_t *req)
{
    if (req->method == HTTP_GET) {
        ESP_LOGI(TAG, "Handshake done, the new connection was opened");
        return ESP_OK;
    }

    httpd_ws_frame_t ws_pkt;
    uint8_t *buf = NULL;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
        return ret;
    }

    if (ws_pkt.len) {
        buf = calloc(1, ws_pkt.len + 1);
        if (buf == NULL) {
            ESP_LOGE(TAG, "Failed to calloc memory for buf");
            return ESP_ERR_NO_MEM;
        }
        ws_pkt.payload = buf;
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
            free(buf);
            return ret;
        }
        ESP_LOGI(TAG, "Got packet with message: %s", ws_pkt.payload);
    }

//    ESP_LOGI(TAG, "frame len is %d", ws_pkt.len);

    /* Treat payload received*/
    if (ws_pkt.type == HTTPD_WS_TYPE_TEXT) {
        treat_payload(&ws_pkt);
        free(buf);
        return trigger_async_send(req->handle, req);
    }
    return ESP_OK;
}

httpd_handle_t http_server_start(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;
    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);

    ESP_LOGI(TAG, "Reading index.html");
    init_web_page_buffer();
    // Set URI handlers
    ESP_LOGI(TAG, "Registering URI handlers");
    static const httpd_uri_t uri_root = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = http_server_get_root_handler,
        .user_ctx  = "ESP32 web server application example for control and monitor field inputs"
    };
    static const httpd_uri_t uri_ws_get = {
        .uri = "/ws",           // URL added to WiFi's default gateway
        .method = HTTP_GET,
        .handler = handle_ws_req,
        .user_ctx = "ESP32 WS",
        .is_websocket = true
    };
    static const httpd_uri_t uri_ws_data_get = {
        .uri = "/ws/data",           // URL added to WiFi's default gateway
        .method = HTTP_GET,
        .handler = http_server_async_get_handler,
        .user_ctx = "ESP32 JSON DATA",
        .is_websocket = true
    };
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &uri_root);
        httpd_register_uri_handler(server, &uri_ws_get);
        httpd_register_uri_handler(server, &uri_ws_data_get);
        httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, &http_server_404_error_handler);
        return server;
    }
    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
};

void http_server_stop(httpd_handle_t *server)
{
    // Stop the httpd server
    httpd_stop(&server);
    ESP_LOGI(TAG, "Server stop!");
};

static void http_server_connect_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    httpd_handle_t *server = (httpd_handle_t *) arg;
    if (NULL == *server) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = http_server_start();
    }
};

static void http_server_disconnect_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    httpd_handle_t *server = (httpd_handle_t *) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        http_server_stop(*server);
        *server = NULL;
    }
};

static esp_err_t http_server_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "404 URI not found");
    ESP_LOGI(TAG, "404 URI not found");
    return ESP_FAIL;
};

static esp_err_t treat_payload(httpd_ws_frame_t *ws_pkt)
{
    if (strcmp((char *) ws_pkt->payload, "btn0") == 0) {
        push_buttons[0].state = !push_buttons[0].state;
    }
    else if (strcmp((char *) ws_pkt->payload, "btn1") == 0) {
        push_buttons[1].state = !push_buttons[1].state;
    }
    else if (strcmp((char *) ws_pkt->payload, "btn2") == 0) {
        push_buttons[2].state = !push_buttons[2].state;
    }
    else if (strcmp((char *) ws_pkt->payload, "btn3") == 0) {
        push_buttons[3].state = !push_buttons[3].state;
    }
    else if (strcmp((char *) ws_pkt->payload, "led0") == 0) {
        leds[0].state = !leds[0].state;
    }
    else if (strcmp((char *) ws_pkt->payload, "led1") == 0) {
        leds[1].state = !leds[1].state;
    }

    return ESP_OK;
}

void init_web_page_buffer(void)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    const char *index_html_path = "/spiffs/index.html";

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        }
        else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    ESP_LOGI(TAG, "Performing SPIFFS_check().");
    ret = esp_spiffs_check(conf.partition_label);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(ret));
        return;
    }
    else {
        ESP_LOGI(TAG, "SPIFFS_check() successful");
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s). Formatting...", esp_err_to_name(ret));
        esp_spiffs_format(conf.partition_label);
        return;
    }
    else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    // Check consistency of reported partiton size info.
    if (used > total) {
        ESP_LOGW(TAG, "Number of used bytes cannot be larger than total. Performing SPIFFS_check().");
        ret = esp_spiffs_check(conf.partition_label);
        // Could be also used to mend broken files, to clean unreferenced pages, etc.
        // More info at https://github.com/pellepl/spiffs/wiki/FAQ#powerlosses-contd-when-should-i-run-spiffs_check
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(ret));
            return;
        }
        else {
            ESP_LOGI(TAG, "SPIFFS_check() successful");
        }
    }

    // Open renamed file for reading
    ESP_LOGI(TAG, "Reading file");

    FILE *file = fopen(index_html_path, "r");
    if (file == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }
    memset((void *) index_html_buffer, 0, sizeof(index_html_buffer));
    struct stat st;
    if (stat(index_html_path, &st)) {
        ESP_LOGE(TAG, "index.html not found");
        return;
    }

    if (fread(index_html_buffer, st.st_size, 1, file) == 0) {
        ESP_LOGE(TAG, "fread failed");
    }
    fclose(file);
    // All done, unmount partition and disable SPIFFS
//    esp_vfs_spiffs_unregister(conf.partition_label);
    ESP_LOGI(TAG, "SPIFFS unmounted");
}