#include "timestamp.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_sntp.h"
#include "esp_eth.h"

void time_sync_notification_cb(struct timeval *tv)
{
    static const char *TAG = "Timestamp";
    ESP_LOGI(TAG, "Notification of a time synchronization event");
}

struct tm get_time_info(void)
{
    time_t now;
    time(&now);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    return timeinfo;
}
void initialize_sntp(void)
{
    static const char *TAG = "Timestamp";
    ESP_LOGI(TAG, "Initializing SNTP");
    esp_sntp_setoperatingmode(ESP_SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
    esp_sntp_init();
    // Set timezone to Brazil Standard Time
    setenv("TZ", "BRT+3", 1);
    tzset();
}

void print_timestamp()
{
    static const char *TAG = "Timestamp";
    char strftime_buf[120];
    struct tm timeinfo = get_time_info();

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t time_us = (int64_t) tv_now.tv_usec;

    strftime(strftime_buf, sizeof(strftime_buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
    ESP_LOGI(TAG, "%s.%lld", strftime_buf, time_us / 1000);
}

timestamp_t get_timestamp()
{
    timestamp_t timestamp;
    timestamp.timeinfo = get_time_info();
    gettimeofday(&timestamp.tv_now, NULL);
    return timestamp;
}

void xTimeStamp(void *arg)
{
    initialize_sntp();

    for (;;) {
//        print_timestamp();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}