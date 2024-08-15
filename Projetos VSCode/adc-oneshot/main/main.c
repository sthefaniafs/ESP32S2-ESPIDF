#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"

const static char *TAG = "ADC TEST";
int adcRaw, voltage;

void app_main(void)
{
    adc_oneshot_unit_handle_t adcHandle1;
    adc_oneshot_unit_init_cfg_t initConfig1 = {
        .unit_id = ADC_UNIT_1
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit (&initConfig1, &adcHandle1));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(adcHandle1, ADC_CHANNEL_0, &config));

    while (1){
        ESP_ERROR_CHECK(adc_oneshot_read(adcHandle1, ADC_CHANNEL_0, &adcRaw));
        ESP_LOGI(TAG, "ADC%d Channel[%d] Raw data: %d", ADC_UNIT_1+1, ADC_CHANNEL_0, adcRaw);
        // valor lido * fundo de escala de tensão / quantidade bits (2^13)
        voltage = (adcRaw * 2500)/ 8192;
        ESP_LOGI(TAG, "ADC%d Channel[%d] Voltage: %d mV", ADC_UNIT_1+1, ADC_CHANNEL_0, voltage);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    
    
}
