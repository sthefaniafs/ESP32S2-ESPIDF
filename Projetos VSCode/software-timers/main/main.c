#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>

#include <driver/gpio.h>
#include "esp_log.h"

#define ledPin1 21
#define ledPin2 14
#define buttonPin 2

static const char *TAG = "SOFTWARE TIMER";

TaskHandle_t xTask1;
TimerHandle_t xTimer1, xTimer2;

void task1 (void * pvParameters);

void timer1_callback(TimerHandle_t xTimer);
void timer2_callback(TimerHandle_t xTimer);

void app_main(void)
{
    gpio_config_t ioConfig = {}; // GPIO config structure

    // Configure LED
    ioConfig.pin_bit_mask = (1ULL<<ledPin1) | (1ULL<<ledPin2); // Bitmask to select the GPIOs you want to configure
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
    ioConfig.intr_type = GPIO_INTR_DISABLE;   // Interrupt on negative edge
    gpio_config(&ioConfig);                   // Configure GPIO with the given settings

    xTimer1 = xTimerCreate("Timer 1", pdMS_TO_TICKS(1000), pdTRUE, (void*)0, timer1_callback); // pdTrue habilita que será um timer periódico
    xTimer2 = xTimerCreate("Timer 2", pdMS_TO_TICKS(5000), pdFALSE, (void*)0, timer2_callback); // pdFALSE configura que será um timer oneshot

    xTaskCreate(task1, "Task1", configMINIMAL_STACK_SIZE + 1024, NULL, 1, &xTask1);

    xTimerStart(xTimer1, 0);

    vTaskDelete(NULL);
}

void task1 (void * pvParameters)
{
    uint8_t debounceTime = 0;

    while (1)
    {
        if(gpio_get_level(buttonPin)==0)
        {
            debounceTime++;
            if(debounceTime>=10)
            {
                gpio_set_level(ledPin2, 1);
                debounceTime=0;
                xTimerStart(xTimer2,0);
                ESP_LOGI(TAG, "TIMER 2 START");
            }
        }
        else{
            debounceTime=0;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    
}

void timer1_callback(TimerHandle_t xTimer)
{
    static uint8_t ledState = 0;
    gpio_set_level(ledPin1, ledState^=1);
    ESP_LOGI(TAG, "TIMER 1 CALLBACK");
}

void timer2_callback(TimerHandle_t xTimer)
{
    gpio_set_level(ledPin2, 0 );
    ESP_LOGI(TAG, "TIMER 2 CALLBACK");
}