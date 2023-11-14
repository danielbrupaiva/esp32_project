#include "defines.h"

#include <stdio.h>
#include <string.h>
// Log
#include "esp_log.h"
#include "esp_system.h"
#include "esp_event.h"
// NVS
#include "nvs.h"
#include "nvs_flash.h"
// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
// Drivers
#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/i2c.h"

static const char *TAG = "MAIN";

static QueueHandle_t gpio_evt_queue = NULL;
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void xGPIO(void *arg)
{
    static const char *TAG = "xGPIO";
    ESP_LOGI(TAG, "Start");
    // configure input port
    gpio_config_t input_conf = {
        .intr_type = GPIO_INTR_DISABLE,      // disable interrupt
        .mode = GPIO_MODE_OUTPUT,            // set as output mode
        .pin_bit_mask = GPIO_OUTPUT_PIN_SEL, // bit mask of the pins that you want to set,e.g.GPIO18/19
        .pull_down_en = 0,                   // disable pull-down mode
        .pull_up_en = 0,                     // disable pull-up mode
    };
    gpio_config(&input_conf);
    // configure output port
    gpio_config_t output_conf = {
        .intr_type = GPIO_INTR_NEGEDGE,     // interrupt of negative edge
        .mode = GPIO_MODE_INPUT,            // set as input mode
        .pin_bit_mask = GPIO_INPUT_PIN_SEL, // bit mask of the pins, e.g use GPIO4/5 here
        .pull_down_en = 1,                  // disable pull-down mode
        .pull_up_en = 1,                    // enable pull-up mode

    };
    gpio_config(&output_conf);
    // configure interrupt
    // create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(int));
    // install gpio isr service
    gpio_install_isr_service(0);
    gpio_isr_handler_add(DIGITAL_INPUT_0, gpio_isr_handler, (void *)DIGITAL_INPUT_0);
    gpio_isr_handler_add(DIGITAL_INPUT_1, gpio_isr_handler, (void *)DIGITAL_INPUT_1);
    ESP_LOGI(TAG, "GPIOs configured");

    uint32_t pin_number;
    for (;;)
    {
        if (xQueueReceive(gpio_evt_queue, &pin_number, portMAX_DELAY))
        {
            switch (pin_number)
            {
            case DIGITAL_INPUT_0:
                gpio_isr_handler_remove(DIGITAL_INPUT_0);
                // call function
                gpio_isr_handler_add(DIGITAL_INPUT_0, gpio_isr_handler, (void *)DIGITAL_INPUT_0);
                break;
            case DIGITAL_INPUT_1:
                gpio_isr_handler_remove(DIGITAL_INPUT_1);
                // call function
                gpio_isr_handler_add(DIGITAL_INPUT_1, gpio_isr_handler, (void *)DIGITAL_INPUT_1);
                break;
            default:
                break;
            }
        }
    }

    vTaskDelete(NULL);
}

void xADC(void *arg)
{
    static const char *TAG = "xADC";
    ESP_LOGI(TAG, "Start");

    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_CH_0, ADC_ATTEN_DB_11);

    float sensor_value = 0.0f;
    for (;;)
    {
        sensor_value = (float)adc1_get_raw(ADC1_CH_0) * 100 / 4095;
    }
    vTaskDelete(NULL);
}
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

#define WIFI_SSID "BRUG.2G"
#define WIFI_PASS ""

static EventGroupHandle_t wifi_event_group;
const static int WIFI_CONNECTED_BIT = BIT0;
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

static void wifi_init_start(void)
{
    static const char *TAG = "xWifi";
    ESP_LOGI(TAG, "Start");
    wifi_event_group = xEventGroupCreate();
    wifi_init_start();
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);

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

    ESP_LOGI(TAG, "wifi_init_start finished.");
    ESP_LOGI(TAG, "connect to ap SSID:%s password", WIFI_SSID);
}

void app_main(void)
{
    ESP_LOGI(TAG, "Start");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    // Register threads

    if (pdTRUE != xTaskCreate(xGPIO, "Task GPIO", 2048, NULL, 0, NULL))
    {
        ESP_LOGI(TAG, "error - nao foi possivel alocar Task GPIO.");
        return;
    }
    if (pdTRUE != xTaskCreate(xADC, "Task ADC", 2048, NULL, 0, NULL))
    {
        ESP_LOGI(TAG, "error - nao foi possivel alocar Task ADC.");
        return;
    }
}
