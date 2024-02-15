#pragma once
/* LwIP SNTP example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_sleep.h"
#include "nvs_flash.h"
#include "esp_sntp.h"
#include "esp_eth.h"

static void obtain_time(void);
static void initialize_sntp(void);

void time_sync_notification_cb(struct timeval *tv)
{
    static const char *TAG = "SNTP";
    ESP_LOGI(TAG, "Notification of a time synchronization event");
}

static void obtain_time(void)
{
}

static void initialize_sntp(void)
{
}

void xTimeStamp()
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

    time_t now;
    char strftime_buf[64];
    struct tm timeinfo;

    for (;;) {

        time(&now);
        localtime_r(&now, &timeinfo);
        strftime(strftime_buf, sizeof(strftime_buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
        ESP_LOGI(TAG, "%s", strftime_buf);

//    TODO: implement microsecond
//    struct timeval tv_now;
//    gettimeofday(&tv_now, NULL);
//    int64_t time_us = (int64_t) tv_now.tv_sec * 1000000L + (int64_t) tv_now.tv_usec;
//    ESP_LOGI(TAG, "%lld us", time_us);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
