#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <driver/gpio.h>
#include "esp_log.h"

#define ledPin1 21
#define ledPin2 33
#define ledPin3 14
#define ledPin4 13
#define buttonPin1 2
#define buttonPin2 3

static const char *TAG = "SEMAPHORE COUNTING";

SemaphoreHandle_t xCountingSemaphore = NULL;

void led_task(void *pvParameters);

static void IRAM_ATTR gpio_isr_handler(void *arg){

    BaseType_t xHigherPriorityTaskWoken = pdTRUE;
    if(buttonPin1 == (uint32_t)arg){
        xSemaphoreGiveFromISR(xCountingSemaphore, &xHigherPriorityTaskWoken);
    }
}

void app_main(void)
{
    gpio_config_t ioConfig = {}; // GPIO config structure

    // Configure LED
    ioConfig.pin_bit_mask = (1ULL<<ledPin1) | (1ULL<<ledPin2) | (1ULL<<ledPin3)| (1ULL<<ledPin4); // Bitmask to select the GPIOs you want to configure
    ioConfig.mode = GPIO_MODE_OUTPUT;      // Set the GPIO mode
    ioConfig.pull_up_en = 0;               // Disable pull-up mode
    ioConfig.pull_down_en = 0;             // Disable pull-down mode
    ioConfig.intr_type = GPIO_INTR_DISABLE; // Disable interrupts
    gpio_config(&ioConfig);                 // Configure GPIO with the given settings

    // Configure button
    ioConfig.pin_bit_mask = (1ULL<<buttonPin1)|(1ULL<<buttonPin2); // Bitmask to select the GPIOs you want to configure
    ioConfig.mode = GPIO_MODE_INPUT;          // Set the GPIO mode
    ioConfig.pull_up_en = 1;                  // Enable pull-up mode
    ioConfig.pull_down_en = 0;                // Disable pull-down mode
    ioConfig.intr_type = GPIO_INTR_NEGEDGE;   // Interrupt on negative edge
    gpio_config(&ioConfig);                   // Configure GPIO with the given settings

    //install gpio isr service and add isr handler for button1 and button2
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);                         //install gpio isr service
    gpio_isr_handler_add(buttonPin1, gpio_isr_handler, (void*) buttonPin1);  //add isr handler for button1
    // gpio_isr_handler_add(buttonPin2, gpioIsrHandler, (void*) buttonPin2); //add isr handler for button2

    xCountingSemaphore = xSemaphoreCreateCounting(255, 0); //maximo e minimo
    xTaskCreate(&led_task, "led task", configMINIMAL_STACK_SIZE + 1024, NULL, 5, NULL); // quanto maior o numero maior a prioridade, 5 é uma prioridade ALTA

    vTaskDelete(NULL);  // Delete this task

}

void led_task(void *pvParameters){

    BaseType_t x = 0; // é o tipo de retorno da função de contagem do semaforo

    while (1)
    {
        if(xSemaphoreTake(xCountingSemaphore, portMAX_DELAY)==pdTRUE){
            x = uxSemaphoreGetCount(xCountingSemaphore);
            gpio_set_level(ledPin1, x%2);
            ESP_LOGI(TAG, "Counting %d", x);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
    
}