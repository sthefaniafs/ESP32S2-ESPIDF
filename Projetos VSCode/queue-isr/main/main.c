#include <stdio.h>
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <driver/gpio.h>
#include "esp_log.h"

#define ledPin1 21
#define ledPin2 33
#define ledPin3 14
#define ledPin4 13
#define buttonPin1 2
#define buttonPin2 3

static const char* TAG = "BUTTON TEST";

QueueHandle_t gpioEvtQueue = NULL;

void button_task (void* pvParameters);
void led_task (void* pvParameters);

static void IRAM_ATTR gpioIsrHandler(void *arg){
    uint32_t gpioNum = (uint32_t) arg;
    BaseType_t xHigherPriorityTaskWoken = pdTRUE;
    xQueueSendFromISR(gpioEvtQueue, &gpioNum, &xHigherPriorityTaskWoken);
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
    gpio_isr_handler_add(buttonPin1, gpioIsrHandler, (void*) buttonPin1);  //add isr handler for button1
    gpio_isr_handler_add(buttonPin2, gpioIsrHandler, (void*) buttonPin2); //add isr handler for button2

    gpioEvtQueue = xQueueCreate(2, sizeof(int32_t));
    xTaskCreate(button_task, "button task", 2048, NULL, 2, NULL);
    xTaskCreate(led_task, "led task", 2048, NULL, 1, NULL);

    while(1){
        gpio_set_level(ledPin3, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_set_level(ledPin3, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }

}

void led_task (void* pvParameters){
    uint8_t LED = 0;
    while (1)
    {
        LED^=1;
        gpio_set_level(ledPin4, LED);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
}


void button_task (void* pvParameters){
    uint32_t gpioNum;
    uint8_t led1 =0, led2 = 0;
    TickType_t lastButtonPress = 0;

    while (1)
    {
        xQueueReceive(gpioEvtQueue, &gpioNum, portMAX_DELAY);
        ESP_LOGI(TAG, "GPIO[%li] intr \n", gpioNum);
        TickType_t currentTime = xTaskGetTickCount();
        
        if(currentTime - lastButtonPress >= pdMS_TO_TICKS(250)){
            lastButtonPress = currentTime;
            if (gpioNum == buttonPin1)
            {
                gpio_set_level(ledPin1, led1^=1);
            }
            else if (gpioNum == buttonPin2)
            {
                gpio_set_level(ledPin2, led2^=1);
            }
        }   
    }
    
}
