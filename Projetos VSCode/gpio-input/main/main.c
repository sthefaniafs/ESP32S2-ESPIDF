#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define ledPin1   14
#define ledPin2   13
#define buttonPin1 2
#define buttonPin2 3

static const char *TAG= "BUTTON TEST";

void app_main(void)
{
    int buttonState1 = 1;
    int buttonState2 = 1;
    bool j = 0;
    bool i = 0;

    gpio_reset_pin(ledPin1);
    gpio_set_direction(ledPin1, GPIO_MODE_DEF_OUTPUT);

    gpio_reset_pin(ledPin2);
    gpio_set_direction(ledPin2, GPIO_MODE_DEF_OUTPUT);

    gpio_reset_pin(buttonPin1);
    gpio_set_direction(buttonPin1, GPIO_MODE_DEF_INPUT);
    gpio_set_pull_mode(buttonPin1, GPIO_PULLUP_ONLY);

    gpio_reset_pin(buttonPin2);
    gpio_set_direction(buttonPin2, GPIO_MODE_DEF_INPUT);
    gpio_set_pull_mode(buttonPin2, GPIO_PULLUP_ONLY);

    ESP_LOGI(TAG, "INIT");

    while (1){
        int newState1 = gpio_get_level(buttonPin1);
        int newState2 = gpio_get_level(buttonPin2);

        if(newState1 != buttonState1){
            buttonState1 = newState1;
            //ESP_LOGI(TAG, "VERIFICOU STATUS");
            if(buttonState1 == 0){
                gpio_set_level(ledPin1, i^=1);
                ESP_LOGI(TAG, "BUTTON 1 PRESSED");
            }
            else{
                ESP_LOGI(TAG, "BUTTON 1 RELEASED");
            }
        }
       if(newState2 != buttonState2){
            buttonState2 = newState2;

            if(buttonState2 == 0){
                gpio_set_level(ledPin2, j^=1);
                ESP_LOGI(TAG, "BUTTON 2 PRESSED");
            }
            else{
                ESP_LOGI(TAG, "BUTTON 2 RELEASED");
            }
        }
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}
