/* 
    Ping Pong entre duas Tasks; 
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * FreeRTOS;
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
/**
 * IDF;
 */
#include "esp_system.h"
#include "esp_log.h"
/**
 * Globais;
 */
static xQueueHandle filaUnica;


/**
 * Ping;
 */
void task_ping (void * pvPortParameter) 
{
    const char * TAG = "task_ping";
    const char * mping = "Ping";
    const char * mpong = NULL; 
    for (;;)
    {
        if (xQueueSend(filaUnica, &mping, portMAX_DELAY) == pdTRUE) {
            vTaskDelay (100/portTICK_PERIOD_MS);
            xQueueReceive(filaUnica, &mpong, portMAX_DELAY);
            ESP_LOGI(TAG, "%s", mpong);            
        }
        
    }
    vTaskDelete(NULL); 
}

/**
 * Pong;
 */
void task_pong (void * pvPortParameter) 
{
    const char * TAG = "task_pong";
    const char * mpong = "Pong";
    const char * mping = NULL;
    for (;;)
    {
        if (xQueueReceive(filaUnica, &mping, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI(TAG, "%s", mping);
            xQueueSend(filaUnica, &mpong, portMAX_DELAY);    
            vTaskDelay (100/portTICK_PERIOD_MS);         
        }
    
    }
    vTaskDelete(NULL); 
}

/**
 * Main;
 */
/*
void app_main (void)
{
    
    filaUnica = xQueueCreate(5, sizeof(char *)); 
    if (filaUnica == NULL) {
        ESP_LOGE(TAG, "error..."); 
        return;
    }


    if (xTaskCreate(task_ping, "task_ping", 2*1024, NULL, 1, NULL) == pdFALSE) {
        ESP_LOGE(TAG, "error..."); 
        return;
    }
 
    if (xTaskCreate(task_pong, "task_pong", 2*1024, NULL, 1, NULL) == pdFALSE) {
        ESP_LOGE(TAG, "error..."); 
        return;
    }   

} */
