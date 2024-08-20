#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/dac_oneshot.h"
#include "esp_log.h"

const static char *TAG = "DAC ONESHOT TEST";


void app_main(void)
{
    dac_oneshot_handle_t chan0Handle; // na franzininho wifi o canal 0 está no pino 17
    dac_oneshot_config_t chan0Config ={
        .chan_id  = DAC_CHAN_0
    };

    ESP_ERROR_CHECK(dac_oneshot_new_channel (&chan0Config, &chan0Handle));
    ESP_LOGI(TAG, "Exemplo DAC oneshot iniciado");


    while (1)
    {
        for (uint8_t i = 0; i < 255; i++) // somente esse for forma a onda dente de serra
        {
            ESP_ERROR_CHECK(dac_oneshot_output_voltage(chan0Handle, i));
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        
         for (uint8_t i = 255; i > 0; i--) // os dois for's formam a onda triangular
        {
            ESP_ERROR_CHECK(dac_oneshot_output_voltage(chan0Handle, i));
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        
        

    }
    

}
