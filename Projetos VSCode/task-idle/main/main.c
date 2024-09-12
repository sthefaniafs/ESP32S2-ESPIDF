#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG1 = "TASK APPMAIN"; 
static const char *TAG2 = "TASK BLINK"; 
static const char *TAG3 = "TASK COUNTER"; 

const unsigned int ledPin1 = 14;
const unsigned int ledPin2 = 21;
const unsigned int blink1 = 1000;
const unsigned int blink2 = 500;

typedef struct {
    int ledPin;
    int blinkPeriod;
}led_t;

led_t led1= {ledPin1, blink1};
led_t led2= {ledPin2, blink2};

void v_task_blink (void * pvParameters);    
void v_task_counter (void * pvParameters);    

TaskHandle_t xTaskBlink1Handle = NULL;
TaskHandle_t xTaskBlink2Handle = NULL;
TaskHandle_t xTaskCounterHandle = NULL;

static uint32_t count = 0UL;
static uint32_t lastCount = 0;


//app_main é uma task 
void app_main(void)
{
    ESP_LOGI(TAG1, "Iniciando programa");
    xTaskCreate(
        v_task_blink, // função da task
        "TaskBlink1", // nome da task
        2048, // tamanho da stack
        // como é uma estrutura passo o endereço, se fosse uma variável normal passaria apenas a variável ex: (void *) ledPin2
        (void *) &led1, // parâmetro para criação da task
        1, // prioridade
        &xTaskBlink1Handle // handle da task onde é atribuido o endereço
    );

    xTaskCreate(
        v_task_blink, // função da task
        "TaskBlink2", // nome da task
        2048, // tamanho da stack
        (void *) &led2,  // parâmetro para criação da task
        1, // prioridade
        &xTaskBlink2Handle // handle da task onde é atribuido o endereço
    );

    xTaskCreate(
        v_task_counter, // função da task
        "TaskCounter", // nome da task
        2048, // tamanho da stack
        (void *) 10, // parâmetro para criação da task
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

    //int pin = (int) pvParameters; // se recebesse uma variável como parâmetro
    //recebendo uma estrutura como parâmetro:
    int pin = ((led_t*) pvParameters)->ledPin;
    int period = ((led_t*) pvParameters)->blinkPeriod;
    bool status = false;

    ESP_LOGI(TAG2, "Iniciando Task Blink");
    gpio_set_direction(pin, GPIO_MODE_DEF_OUTPUT);

    while (1)
    {
        gpio_set_level(pin, status^=1);
        ESP_LOGI(TAG2, "[%s] LED[%d]: %d", pcTaskGetName(NULL), pin, status);
        vTaskDelay(pdMS_TO_TICKS(period));
    } 
}

void v_task_counter (void * pvParameters){

    int i = (int) pvParameters;

    while (1)
    {
        ESP_LOGI(TAG3, "Counter: %d", i++);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }   

}

void vApplicationIdleHook(void)
{
    count ++;
    if(count - lastCount >= pdMS_TO_TICKS(1000))
    {
        ESP_LOGI(__func__, "Idle cycle count: %lu", count);
        lastCount = count; 
    }
}