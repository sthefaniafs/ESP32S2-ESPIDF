#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "calculadora.h"

void app_main(void)
{
    int resultado;
    liga_led();
    resultado = soma(2,5);
    printf("Resultado: %d\n", resultado);
    while(1)
    {
        printf("Hello world!\n");
        vTaskDelay(500/portTICK_PERIOD_MS);
    }
    
}