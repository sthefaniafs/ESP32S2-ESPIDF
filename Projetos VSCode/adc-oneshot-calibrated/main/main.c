#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"

const static char *TAG = "ADC TEST";
int adcRaw, voltage;
static bool example_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle); // ADC calibration init
static void example_adc_calibration_deinit(adc_cali_handle_t handle);                                                               // ADC calibration deinit

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

    adc_cali_handle_t adcCaliChan0Handle = NULL;
    bool doCalibration1Chan0 = example_adc_calibration_init(ADC_UNIT_1, ADC_CHANNEL_0, ADC_ATTEN_DB_11, &adcCaliChan0Handle); // ADC1 Calibration Init

    while (1){
        ESP_ERROR_CHECK(adc_oneshot_read(adcHandle1, ADC_CHANNEL_0, &adcRaw));
        ESP_LOGI(TAG, "ADC%d Channel[%d] Raw data: %d", ADC_UNIT_1+1, ADC_CHANNEL_0, adcRaw);
        // valor lido * fundo de escala de tensão / quantidade bits (2^13)
        if (doCalibration1Chan0){
            ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adcCaliChan0Handle, adcRaw, &voltage));
            ESP_LOGI(TAG, "ADC%d Channel[%d] Voltage: %d mV", ADC_UNIT_1+1, ADC_CHANNEL_0, voltage);
        }   
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

      // Tear Down
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adcHandle1));             // ADC1 Deinit
    if (doCalibration1Chan0)                                      // ADC1 Calibration Deinit
    {
        example_adc_calibration_deinit(adcCaliChan0Handle);     // ADC1 Calibration Deinit
    }
    
    
}

/*---------------------------------------------------------------
        ADC Calibration
---------------------------------------------------------------*/
static bool example_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated)
    {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Curve Fitting");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .chan = channel,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (ret == ESP_OK)
        {
            calibrated = true;
        }
    }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated)
    {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK)
        {
            calibrated = true;
        }
    }
#endif

    *out_handle = handle;
    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "Calibration Success");
    }
    else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated)
    {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    }
    else
    {
        ESP_LOGE(TAG, "Invalid arg or no memory");
    }

    return calibrated;
}

static void example_adc_calibration_deinit(adc_cali_handle_t handle)
{
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "deregister %s calibration scheme", "Curve Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(handle));

#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "deregister %s calibration scheme", "Line Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(handle));
#endif
}