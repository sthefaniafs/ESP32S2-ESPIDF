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

static const char *TAG = "SEMAPHORE MUTEX";

SemaphoreHandle_t xMutexSemaphore = NULL;

void task1(void *pvParameters);
void task2(void *pvParameters);

void send_serial_data(char *data){
    printf(data);
}

void app_main(void)
{
    xMutexSemaphore = xSemaphoreCreateMutex(); //maximo e minimo
    xTaskCreate(&task1, "task 1", 2048, NULL, 1, NULL); // quanto maior o numero maior a prioridade, baixa prioridade
    xTaskCreate(&task2, "task 2", 2048, NULL, 4, NULL); // quanto maior o numero maior a prioridade, alta prioridade

    vTaskDelete(NULL);  // Delete this task

}

void task1(void *pvParameters){

    while (1)
    {
        xSemaphoreTake(xMutexSemaphore, portMAX_DELAY);
        ESP_LOGI(TAG, "TASK 1 PEGOU O SEMAFORO MUTEX");
        send_serial_data("Task 1: [1] Enviando informações pela interface Serial\n"); // Send data to serial interface
        send_serial_data("Task 1: [2] Enviando informações pela interface Serial\n"); // Send data to serial interface
        send_serial_data("Esperando 2 segundos...\n");                                // Send data to serial interface
        for(int i = 0;i<20000;i++){
            esp_rom_delay_us(100);
        }
        send_serial_data("Task 1: [3] Enviando informações pela interface Serial\n"); // Send data to serial interface
        send_serial_data("Task 1: [4] Enviando informações pela interface Serial\n"); // Send data to serial interface
        ESP_LOGI(TAG, "Task 1 LIBEROU O SEMAFORO MUTEX"); 
        xSemaphoreGive(xMutexSemaphore); 
        vTaskDelay(1000 / portTICK_PERIOD_MS);                                       // Delay 100ms  
       xSemaphoreGive(xMutexSemaphore);
    }
    
}
void task2(void *pvParameters){
    while (1)
    {
        xSemaphoreTake(xMutexSemaphore, portMAX_DELAY);                             // Take the semaphore             
        ESP_LOGI(TAG, "Task 2: PEGOU O SEMAFORO MUTEX");                            // Log the event
        send_serial_data("Task 2: [1] Enviando informações pela interface Serial\n"); // Send data to serial interface
        send_serial_data("Task 2: [2] Enviando informações pela interface Serial\n"); // Send data to serial interface
        send_serial_data("Task 2: [3] Enviando informações pela interface Serial\n"); // Send data to serial interface
        send_serial_data("Task 2: [4] Enviando informações pela interface Serial\n"); // Send data to serial interface
        send_serial_data("Task 2: [5] Enviando informações pela interface Serial\n"); // Send data to serial interface
        ESP_LOGI(TAG, "Task 2: LIBEROU O SEMAFORO MUTEX");                          // Log the event
        xSemaphoreGive(xMutexSemaphore);                                            // Give the semaphore
        vTaskDelay(100 / portTICK_PERIOD_MS);                                       // Delay 100ms
    }

}