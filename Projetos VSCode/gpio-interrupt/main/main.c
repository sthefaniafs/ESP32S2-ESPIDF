#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define ledPin1   21
#define ledPin2   33
#define ledPin3   14
#define buttonPin1 2
#define buttonPin2 3

static const char* TAG = "BUTTON TEST";
static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler (void* arg){
    uint32_t gpioNum = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpioNum, NULL);
} 

void buttonTask (void* pvParameters){
    uint32_t gpioNum;
    TickType_t lastPress = 0;
    bool buttonState2 = 0;

    while (1){
        xQueueReceive(gpio_evt_queue, &gpioNum, portMAX_DELAY);
        ESP_LOGI(TAG, "GPIO[%li] intr\n", gpioNum);

        TickType_t currentTime = xTaskGetTickCount();

        if (currentTime -  lastPress >= pdMS_TO_TICKS(250)){
            lastPress = currentTime;

            if (gpioNum == buttonPin1){
                bool buttonState = gpio_get_level(buttonPin1);
                gpio_set_level(ledPin1, buttonState == 0 ? 1 : 0 );
            }
            else if(gpioNum == buttonPin2){
                gpio_set_level(ledPin2, buttonState2^=1);
            }  
        }  
    }
}
void app_main(void)
{
    //Configurando leds 
    gpio_reset_pin(ledPin1);
    gpio_set_direction(ledPin1, GPIO_MODE_DEF_OUTPUT);

    gpio_reset_pin(ledPin2);
    gpio_set_direction(ledPin2, GPIO_MODE_DEF_OUTPUT);

    gpio_reset_pin(ledPin3);
    gpio_set_direction(ledPin3, GPIO_MODE_DEF_OUTPUT);

    // define leds iniciando apagados
    gpio_set_level(ledPin1, 0);
    gpio_set_level(ledPin2, 0);
    gpio_set_level(ledPin3, 0);

    //Configurando botoes
    gpio_reset_pin(buttonPin1);
    gpio_set_direction(buttonPin1, GPIO_MODE_DEF_INPUT);
    gpio_set_pull_mode(buttonPin1, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(buttonPin1, GPIO_INTR_ANYEDGE);

    gpio_reset_pin(buttonPin2);
    gpio_set_direction(buttonPin2, GPIO_MODE_DEF_INPUT);
    gpio_set_pull_mode(buttonPin2, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(buttonPin2, GPIO_INTR_NEGEDGE);

    // configurando interrupcao
    gpio_evt_queue = xQueueCreate(1, sizeof(uint32_t));
    xTaskCreate(buttonTask, "BUTTON TASK", 2048, NULL, 2, NULL);

    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
    gpio_isr_handler_add(buttonPin1, gpio_isr_handler, (void*)buttonPin1);
    gpio_isr_handler_add(buttonPin2, gpio_isr_handler, (void*)buttonPin2);

    while (1){
        gpio_set_level(ledPin3, 1);
        vTaskDelay(500/portTICK_PERIOD_MS);
        gpio_set_level(ledPin3, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }


}
