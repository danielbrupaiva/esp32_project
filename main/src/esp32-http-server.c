#include "esp32-http-server.h"
#include "payload.h"
#include "timestamp.h"
#include "esp32-gpio.h"
#define MIN(a, b) ((a) < (b) ? (a) : (b))
/* HTTP server */

static esp_err_t http_server_get_led0_handler(httpd_req_t *req)
{
    static const char *TAG = "HTTP_SERVER";
    const char *response = "Toggle LED0";
    leds[0].state = !leds[0].state;
    esp_err_t error = httpd_resp_send(req, response, strlen(response));

    if (ESP_OK != error) {
        ESP_LOGI(TAG, "Erro %d while sending response", error);
        return error;
    }
    ESP_LOGI(TAG, "Response sent successfully");
    return error;
};
static esp_err_t http_server_get_led1_handler(httpd_req_t *req)
{
    static const char *TAG = "HTTP_SERVER";
    const char *response = "Toggle LED1";
    leds[1].state = !leds[1].state;
    esp_err_t error = httpd_resp_send(req, response, strlen(response));

    if (ESP_OK != error) {
        ESP_LOGI(TAG, "Erro %d while sending response", error);
        return error;
    }
    ESP_LOGI(TAG, "Response sent successfully");
    return error;
};
static esp_err_t http_server_get_push_button0_handler(httpd_req_t *req)
{
    static const char *TAG = "HTTP_SERVER";
    const char *response = "Toggle push_button0 state";
    push_buttons[0].state = !push_buttons[0].state;
    esp_err_t error = httpd_resp_send(req, response, strlen(response));

    if (ESP_OK != error) {
        ESP_LOGI(TAG, "Erro %d while sending response", error);
        return error;
    }
    ESP_LOGI(TAG, "Response sent successfully");
    return error;
};
static esp_err_t http_server_get_push_button1_handler(httpd_req_t *req)
{
    static const char *TAG = "HTTP_SERVER";
    const char *response = "Toggle push_button1 state";
    push_buttons[1].state = !push_buttons[1].state;
    esp_err_t error = httpd_resp_send(req, response, strlen(response));

    if (ESP_OK != error) {
        ESP_LOGI(TAG, "Erro %d while sending response", error);
        return error;
    }
    ESP_LOGI(TAG, "Response sent successfully");
    return error;
};
static esp_err_t http_server_get_push_button2_handler(httpd_req_t *req)
{
    static const char *TAG = "HTTP_SERVER";
    const char *response = "Toggle push_button2 state";
    push_buttons[2].state = !push_buttons[2].state;
    esp_err_t error = httpd_resp_send(req, response, strlen(response));

    if (ESP_OK != error) {
        ESP_LOGI(TAG, "Erro %d while sending response", error);
        return error;
    }
    ESP_LOGI(TAG, "Response sent successfully");
    return error;
};
static esp_err_t http_server_get_push_button3_handler(httpd_req_t *req)
{
    static const char *TAG = "HTTP_SERVER";
    const char *response = "Toggle push_button3 state";
    push_buttons[3].state = !push_buttons[3].state;
    esp_err_t error = httpd_resp_send(req, response, strlen(response));

    if (ESP_OK != error) {
        ESP_LOGI(TAG, "Erro %d while sending response", error);
        return error;
    }
    ESP_LOGI(TAG, "Response sent successfully");
    return error;
};
static esp_err_t http_server_get_root_handler(httpd_req_t *req)
{
    static const char *TAG = "HTTP_SERVER";
    const char *response = "<!DOCTYPE html>\n"
                           "<html lang=\"en\">\n"
                           "<head>\n"
                           "    <meta charset=\"UTF-8\">\n"
                           "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                           "    <title>ESP32 WebServer</title>\n"
                           "    <div id=\"top\">\n"
                           "        <img class=\"logo\" src=\"https://www.sp.senai.br/images/senai.svg\" alt=\"logo\">\n"
                           "        <h1>ESP32 WebServer application</h1>\n"
                           "        <h2>Author: Daniel Paiva</h2>\n"
                           "        <h>WebServer aplication using ESP32 for remotely control and monitoring sensors at field</h>\n"
                           "        <hr>\n"
                           "\n"
                           "    </div>\n"
                           "    <!-- Add icon library -->\n"
                           "    <link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css\">\n"
                           "    <link rel=\"stylesheet\" href=\"https://fonts.googleapis.com/icon?family=Material+Icons\">\n"
                           "    <style>\n"
                           "        .led{\n"
                           "            position: relative;\n"
                           "            display: inline-grid;\n"
                           "            margin: 20px 10px;\n"
                           "            padding: 24px;\n"
                           "\n"
                           "            border: none;\n"
                           "            cursor: pointer;\n"
                           "            text-decoration: none;\n"
                           "            color: #333;\n"
                           "            background-color: #ecf0f1;\n"
                           "            transition: background-color .3s;\n"
                           "        }\n"
                           "        .btn {\n"
                           "            position: relative;\n"
                           "            display: inline-grid;\n"
                           "            margin: 20px 10px;\n"
                           "            padding: 24px;\n"
                           "\n"
                           "            overflow: hidden;\n"
                           "            cursor: pointer;\n"
                           "            border: none;\n"
                           "            border-radius: 2px;\n"
                           "            outline: none;\n"
                           "\n"
                           "            box-shadow: 0 1px 4px rgba(0, 0, 0, .6);\n"
                           "\n"
                           "            background-color: #2ecc71;\n"
                           "            color: #ecf0f1;\n"
                           "\n"
                           "            transition: background-color .3s;\n"
                           "        }\n"
                           "        .btn:hover {\n"
                           "            background-color: #27ae60;\n"
                           "        }\n"
                           "        .btn > * {\n"
                           "            position: relative;\n"
                           "        }\n"
                           "\n"
                           "        .btn span {\n"
                           "            display: inline-grid;\n"
                           "            padding: 12px 24px;\n"
                           "        }\n"
                           "        .btn:before {\n"
                           "            content: \"\";\n"
                           "            position: relative;\n"
                           "            top: 50%;\n"
                           "            left: 50%;\n"
                           "            display: inline-grid;\n"
                           "            width: 0;\n"
                           "            padding-top: 0;\n"
                           "            border-radius: 100%;\n"
                           "            background-color: rgba(236, 240, 241, .3);\n"
                           "            -webkit-transform: translate(-50%, -50%);\n"
                           "            -moz-transform: translate(-50%, -50%);\n"
                           "            -ms-transform: translate(-50%, -50%);\n"
                           "            -o-transform: translate(-50%, -50%);\n"
                           "            transform: translate(-50%, -50%);\n"
                           "        }\n"
                           "        html {\n"
                           "            position: relative;\n"
                           "            height: 100%;\n"
                           "        }\n"
                           "        h2 {\n"
                           "            font-weight: normal;\n"
                           "        }\n"
                           "        body {\n"
                           "            position: relative;\n"
                           "            top: 0%;\n"
                           "            left: 0%;\n"
                           "\n"
                           "            /*-webkit-transform: translate(-50%, -50%);*/\n"
                           "            /*-moz-transform: translate(-50%, -50%);*/\n"
                           "            /*-ms-transform: translate(-50%, -50%);*/\n"
                           "            /*-o-transform: translate(-50%, -50%);*/\n"
                           "            /*transform: translate(-50%, -50%);*/\n"
                           "\n"
                           "            background-color: #ecf0f1;\n"
                           "            color: #34495e;\n"
                           "            font-family: Trebuchet, Arial, sans-serif;\n"
                           "            text-align: center;\n"
                           "        }\n"
                           "\n"
                           "        .success {background-color: #04AA6D;} /* Green */\n"
                           "        .success:hover {background-color: #46a049;}\n"
                           "        .info {background-color: #2196F3;} /* Blue */\n"
                           "        .info:hover {background: #0b7dda;}\n"
                           "        .warning {background-color: #ff9800;} /* Orange */\n"
                           "        .warning:hover {background: #e68a00;}\n"
                           "        .danger {background-color: #f44336;} /* Red */\n"
                           "        .danger:hover {background: #da190b;}\n"
                           "        .default {background-color: #e7e7e7; color: black;} /* Gray */\n"
                           "        .default:hover {background: #ddd;}\n"
                           "\n"
                           "        .material-icons {\n"
                           "            vertical-align: middle;\n"
                           "        }\n"
                           "\n"
                           "    </style>\n"
                           "</head>\n"
                           "<body>\n"
                           "<div id=\"gpios\" class=\"gpios\">\n"
                           "    <h2>GPIO</a></h2>\n"
                           "    <p>Push buttons [ON/OFF]</p>\n"
                           "    <div class=\"buttons-container\">\n"
                           "        <button id=\"btn0\" class=\"btn\" type=\"button\">BTN0</button>\n"
                           "        <button id=\"btn1\" class=\"btn\" type=\"button\">BTN1</button>\n"
                           "        <button id=\"btn2\" class=\"btn\" type=\"button\">BTN2</button>\n"
                           "        <button id=\"btn3\" class=\"btn\" type=\"button\">BTN3</button>\n"
                           "        <hr>\n"
                           "    </div>\n"
                           "\n"
                           "    <div class=\"leds-container\">\n"
                           "    <p>LEDs</p>\n"
                           "        <button class=\"led\">LED0\n"
                           "            <span id=\"led0\" class=\"material-icons\">radio_button_unchecked</span>\n"
                           "        </button>\n"
                           "        <button class=\"led\">LED1\n"
                           "            <span id=\"led1\" class=\"material-icons\">radio_button_unchecked</span>\n"
                           "        </button>\n"
                           "    </div>\n"
                           "</div>\n"
                           "    <hr>\n"
                           "<div id=\"adc_sensors\" class=\"adc_sensors\">\n"
                           "    <h2>Analog sensor</h2>\n"
                           "    <p>Analog input sensor [0-5V]: <b>100.0%</b></p>\n"
                           "    <hr>\n"
                           "</div>\n"
                           "<div id=\"i2c\" class=\"i2c\">\n"
                           "    <h2>MPU6050 accel/gyro module</h2>\n"
                           "    <p>MPU6050 module as I2C communication example</p>\n"
                           "    <div id=\"accel\" >\n"
                           "        <p>axis_x : <b>12.34</b></p>\n"
                           "        <p>axis_y : <b>12.34</b></p>\n"
                           "        <p>axis_z : <b>12.34</b></p>\n"
                           "    </div>\n"
                           "    <br>\n"
                           "    <div id=\"gyro\">\n"
                           "        <p>gyro_x : <b>12.34</b></p>\n"
                           "        <p>gyro_y : <b>12.34</b></p>\n"
                           "        <p>gyro_z : <b>12.34</b></p>\n"
                           "    </div>\n"
                           "\n"
                           "    <hr>\n"
                           "</div>\n"
                           "\n"
                           "<footer>\n"
                           "    <a href=\"#top\">HOME</a>\n"
                           "    <a href=\"#gpios\">GPIOs</a>\n"
                           "    <a href=\"#adc_sensors\">ADC</a>\n"
                           "    <a href=\"#i2c\">MPU6050</a>\n"
                           "</footer>\n"
                           "<script>\n"
                           "    // WebSocket connection\n"
                           "    var gateway = `ws://${window.location.hostname}/ws`;\n"
                           "    var websocket;\n"
                           "    window.addEventListener('load', onLoad);\n"
                           "    function onLoad(event) {\n"
                           "        initWebSocket();\n"
                           "        initButton();\n"
                           "    }\n"
                           "    function initWebSocket() {\n"
                           "        console.log('Trying to open a WebSocket connection...');\n"
                           "        websocket = new WebSocket(gateway);\n"
                           "        websocket.onopen = onOpen;\n"
                           "        websocket.onclose = onClose;\n"
                           "        websocket.onmessage = onMessage; // <-- add this line\n"
                           "    }\n"
                           "    function onOpen(event) {\n"
                           "        console.log('Connection opened');\n"
                           "    }\n"
                           "    function onClose(event) {\n"
                           "        console.log('Connection closed');\n"
                           "        setTimeout(initWebSocket, 2000);\n"
                           "    }\n"
                           "    function onMessage(event){\n"
                           "        console.log(event.data)\n"
                           "    }\n"
                           "    function initButton(){\n"
                           "        document.getElementsByClassName('button');\n"
                           "        document.getElementById('btn0').addEventListener('click', toggle);\n"
                           "        document.getElementById('btn1').addEventListener('click', toggle);\n"
                           "        document.getElementById('btn2').addEventListener('click', toggle);\n"
                           "        document.getElementById('btn3').addEventListener('click', toggle);\n"
                           "        document.getElementById('led0').addEventListener('click', toggleLED);\n"
                           "        document.getElementById('led1').addEventListener('click', toggleLED);\n"
                           "    }\n"
                           "    function toggle(msg){\n"
                           "        websocket.send('toggle');\n"
                           "        console.log(msg.target.id);\n"
                           "    }\n"
                           "\n"
                           "    function toggleLED(msg){\n"
                           "        //document.getElementById(msg.target.id).textContent = 'radio_button_checked';\n"
                           "        var radioIcon = document.getElementById(msg.target.id).textContent;\n"
                           "        if(radioIcon === 'radio_button_checked'){\n"
                           "            document.getElementById(msg.target.id).textContent = 'radio_button_unchecked';\n"
                           "        }\n"
                           "        else{\n"
                           "            document.getElementById(msg.target.id).textContent = 'radio_button_checked';\n"
                           "        }\n"
                           "        console.log(msg.target.id);\n"
                           "    }\n"
                           "\n"
                           "</script>\n"
                           "</body>\n"
                           "</html>\n"
                           "";

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

    leds[0].state = !leds[0].state;
    gpio_set_level(leds[0].pin_number, leds[0].state);

    char buff[4];
    memset(buff, 0, sizeof(buff));
    sprintf(buff, "%d", leds[0].state);

    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = (uint8_t *) buff;
    ws_pkt.len = strlen(buff);
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
    static const char *TAG = "HTTP_SERVER";
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

    ESP_LOGI(TAG, "frame len is %d", ws_pkt.len);

    if (ws_pkt.type == HTTPD_WS_TYPE_TEXT &&
        strcmp((char *) ws_pkt.payload, "toggle") == 0) {
        free(buf);
        return trigger_async_send(req->handle, req);
    }
    return ESP_OK;
}

httpd_handle_t http_server_start(void)
{
    static const char *TAG = "HTTP_SERVER";

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);

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
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &uri_root);
        httpd_register_uri_handler(server, &uri_ws_get);
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

