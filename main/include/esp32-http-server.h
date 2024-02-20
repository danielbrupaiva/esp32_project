#pragma once

#include <esp_log.h>
#include <esp_system.h>
#include <esp_event.h>
#include <esp_event_loop.h>
#include <freertos/event_groups.h>
#include <esp_http_server.h>

#ifdef __cplusplus
extern "C" {
#endif
/** HTTP server **/
/** Asynchronous response data structure **/
struct async_resp_arg
{
    httpd_handle_t hd; // Server instance
    int fd;            // Session socket file descriptor
};
httpd_handle_t http_server_start(void);
static void http_server_stop(httpd_handle_t server);
static void http_server_connect_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void http_server_disconnect_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static esp_err_t http_server_get_root_handler(httpd_req_t *req);
static esp_err_t http_server_async_get_handler(httpd_req_t *req);
static esp_err_t http_server_404_error_handler(httpd_req_t *req, httpd_err_code_t err);

#ifdef __cplusplus
}
#endif
