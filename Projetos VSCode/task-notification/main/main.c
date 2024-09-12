#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "driver/gpio.h"
#include "esp_log.h"

#define ledPin1 21
#define ledPin2 14
#define buttonPin 2

static const char *TAG = "TASK NOTIFICATIONS EXAMPLE";

TaskHandle_t xTaskHandle1 = NULL;

void vTask1 (void *pvParameters);

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdTRUE;
    if(buttonPin==(uint32_t) arg)
    {
        xTaskNotifyFromISR(xTaskHandle1, 1, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
    }
    
}

void app_main(void)
{
    gpio_config_t ioConfig = {}; // GPIO config structure

    // Configure LED
    ioConfig.pin_bit_mask = (1ULL<<ledPin1); // Bitmask to select the GPIOs you want to configure
    ioConfig.mode = GPIO_MODE_OUTPUT;      // Set the GPIO mode
    ioConfig.pull_up_en = 0;               // Disable pull-up mode
    ioConfig.pull_down_en = 0;             // Disable pull-down mode
    ioConfig.intr_type = GPIO_INTR_DISABLE; // Disable interrupts
    gpio_config(&ioConfig);                 // Configure GPIO with the given settings

    // Configure button
    ioConfig.pin_bit_mask = (1ULL<<buttonPin); // Bitmask to select the GPIOs you want to configure
    ioConfig.mode = GPIO_MODE_INPUT;          // Set the GPIO mode
    ioConfig.pull_up_en = 1;                  // Enable pull-up mode
    ioConfig.pull_down_en = 0;                // Disable pull-down mode
    ioConfig.intr_type = GPIO_INTR_NEGEDGE;   // Interrupt on negative edge
    gpio_config(&ioConfig);                   // Configure GPIO with the given settings

    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
    gpio_isr_handler_add(buttonPin, gpio_isr_handler, (void*)buttonPin);

    xTaskCreate(vTask1, "Task 1", configMINIMAL_STACK_SIZE + 1024, NULL, 2, &xTaskHandle1);
    vTaskDelete(NULL);

}

void vTask1 (void *pvParameters)
{
    uint32_t ulNotifyValue;
    while (1)
    {
        ulNotifyValue = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        
        if (ulNotifyValue==1)
        {
            ESP_LOGI(TAG, "BUTTON PRESSED");
            gpio_set_level(ledPin1, 1);
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            gpio_set_level(ledPin1, 0);

        }
        
    }
}
