#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define ledPin1 21

void app_main(void)
{
    gpio_reset_pin(ledPin1);
    gpio_set_direction(ledPin1, GPIO_MODE_DEF_OUTPUT);


    while (1)
    {
        gpio_set_level(ledPin1, 1);
        printf("LED ON\n");
        vTaskDelay(1000/portTICK_PERIOD_MS);

        gpio_set_level(ledPin1, 0);
        printf("LED OFF\n");
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
    

}
