#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG1 = "TASK APPMAIN"; 
static const char *TAG2 = "TASK BLINK"; 
static const char *TAG3 = "TASK COUNTER"; 

const unsigned int ledPin = 14;

void v_task_blink (void * pvParameters);    
void v_task_counter (void * pvParameters);    

TaskHandle_t xTaskBlinkHandle = NULL;
TaskHandle_t xTaskCounterHandle = NULL;

//app_main é uma task 
void app_main(void)
{
    ESP_LOGI(TAG1, "Iniciando programa");
    xTaskCreate(
        v_task_blink, // função da task
        "TaskBlink", // nome da task
        2048, // tamanho da stack
        NULL, // parâmetro para criação da task
        1, // prioridade
        &xTaskBlinkHandle // handle da task onde é atribuido o endereço
    );

    xTaskCreate(
        v_task_counter, // função da task
        "TaskCounter", // nome da task
        2048, // tamanho da stack
        NULL, // parâmetro para criação da task
        2, // prioridade (quanto maior mais prioridade)
        &xTaskCounterHandle // handle da task onde é atribuido o endereço
    );

    int i = 0;

    while (1)
    {
        ESP_LOGI(TAG1, "Task Main - Counter: %d", i++);   
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }

}

void v_task_blink (void * pvParameters){

    bool status = false;
    ESP_LOGI(TAG2, "Iniciando Task Blink");
    gpio_set_direction(ledPin, GPIO_MODE_DEF_OUTPUT);

    while (1)
    {
        gpio_set_level(ledPin, status^=1);
        ESP_LOGI(TAG2, "Task Blink LED[%d]: %d", ledPin, status);
        vTaskDelay(pdMS_TO_TICKS(1000));
    } 
}

void v_task_counter (void * pvParameters){

    int i = 100;

    while (1)
    {
        ESP_LOGI(TAG3, "Counter: %d", i++);
        if(i==110){
            vTaskDelete(xTaskBlinkHandle);
            gpio_set_level(ledPin, 0);
 
        }
        else if (i==115){
            // como foi deletada ela precisa ser criada novamente para ser usada
            xTaskCreate(
                v_task_blink, // função da task
                "TaskBlink", // nome da task
                2048, // tamanho da stack
                NULL, // parâmetro para criação da task
                1, // prioridade
                &xTaskBlinkHandle // handle da task onde é atribuido o endereço
            );

            vTaskDelete(NULL); // ja entende que é deletar a própria task que solicitou
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    

}