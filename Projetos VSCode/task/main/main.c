#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "TEST TASK: "; 
const unsigned int ledPin = 14;

void v_task_blink (void * pvParameters);    

TaskHandle_t xTaskBlinkHandle = NULL;

//app_main é uma task 
void app_main(void)
{
    ESP_LOGI(TAG, "Iniciando programa");
    xTaskCreate(
        v_task_blink, // função da task
        "TaskBlink", // nome da task
        2048, // tamanho da stack
        NULL, // parâmetro para criação da task
        1, // prioridade
        &xTaskBlinkHandle // handle da task onde é atribuido o endereço
    );

    int i = 0;

    while (1)
    {
        ESP_LOGI(TAG, "Task Main - Counter: %d", i++);   
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }

}

void v_task_blink (void * pvParameters){

    bool status = false;
    ESP_LOGI(TAG, "Iniciando Task Blink");
    gpio_set_direction(ledPin, GPIO_MODE_DEF_OUTPUT);

    while (1)
    {
        gpio_set_level(ledPin, status^=1);
        ESP_LOGI(TAG, "Task Blink LED[%d]: %d", ledPin, status);
        vTaskDelay(pdMS_TO_TICKS(1000));

    }
    

}