#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/dac_cosine.h"
#include "esp_log.h"

const static char *TAG = "DAC COSINE TEST";

void app_main(void)
{
    dac_cosine_handle_t chan0Handle;
    dac_cosine_config_t chan0Config = {
        .chan_id = DAC_CHAN_0,                  // canal 0 = pino 17
        .freq_hz = 1000,                        // 1kHz
        .clk_src = DAC_COSINE_CLK_SRC_DEFAULT,  // default
        .offset = 0,                            // sem offset
        .phase = DAC_COSINE_PHASE_0,            // sem ajuste de fase = 0V
        .atten = DAC_COSINE_ATTEN_DEFAULT,      // atenuação padrão
        .flags.force_set_freq = false           // sem forçar frequencia 
    };

    ESP_LOGI(TAG, "...........Exemplo DAC cosine iniciando...........");

    ESP_ERROR_CHECK(dac_cosine_new_channel (&chan0Config, &chan0Handle));
    ESP_ERROR_CHECK(dac_cosine_start(chan0Handle));
    ESP_LOGI(TAG, "Exemplo DAC cosine iniciado");

}
