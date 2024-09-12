#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <freertos/event_groups.h>

#include <driver/gpio.h>
#include <esp_log.h>

#define ledPin1 21
#define ledPin2 14
#define buttonPin 2

#define task1Bit (1<<0)
#define task2Bit (1<<1)

const static char *TAG = "EVENT GROUP EXAMPLE";

TaskHandle_t xTask1, xTask2, xTask3;
TimerHandle_t xTimer1, xTimer2;

EventGroupHandle_t xEvent;

void vTask1 (void *pvParameters);
void vTask2 (void *pvParameters);
void vTask3 (void *pvParameters);

void timer1_callback (TimerHandle_t pxTimer)
{
    static int count = 0;
    count++;
    gpio_set_level(ledPin1, count%2);
    
    if(count == 5)
    {
        xEventGroupSetBits(xEvent, task1Bit);
    }
    else if(count == 10)
    {
        xEventGroupSetBits(xEvent, task2Bit);

    }
    else if(count == 15)
    {
        count = 0;
        xEventGroupSetBits(xEvent, task1Bit | task2Bit);

    }



}

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
    gpio_config(&ioConfig);                   // Configure GPIO

    xEvent = xEventGroupCreate();
    xTimer1 = xTimerCreate("Timer 1", pdMS_TO_TICKS(1000), pdTRUE, (void *)0, timer1_callback);

    xTaskCreate(vTask1, "Task 1", configMINIMAL_STACK_SIZE + 1024, NULL, 1, &xTask1);
    xTaskCreate(vTask2, "Task 2", configMINIMAL_STACK_SIZE + 1024, NULL, 1, &xTask2);
    xTaskCreate(vTask3, "Task 3", configMINIMAL_STACK_SIZE + 1024, NULL, 1, &xTask3);

    xTimerStart(xTimer1, 0);
    
    vTaskDelete(NULL);

}

void vTask1 (void *pvParameters)
{
    while (1)
    {
        xEventGroupWaitBits(xEvent, task1Bit, pdTRUE, pdTRUE, portMAX_DELAY);
        ESP_LOGI(TAG, "TASK 1 SAIU DO BLOQUEIO");
    }
    
}

void vTask2 (void *pvParameters)
{
    int led = 0;
    while (1)
    {
        xEventGroupWaitBits(xEvent, task2Bit, pdTRUE, pdTRUE, portMAX_DELAY);
        gpio_set_level(ledPin2, led^=1);
        ESP_LOGI(TAG, "TASK 2 SAIU DO BLOQUEIO");

    }
    
}

void vTask3 (void *pvParameters)
{
    while (1)
    {
        xEventGroupWaitBits(xEvent, task2Bit | task1Bit, pdTRUE, pdTRUE, portMAX_DELAY);
        ESP_LOGI(TAG, "TASK 3 SAIU DO BLOQUEIO");

    }
    
}