/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/temperature_sensor.h"

static const char *TAG = "INTERNAL TEMPERATURE SENSOR";

void app_main(void)
{
    ESP_LOGI(TAG, "Install temperature sensor, expected temp ranger range: 10~50 ℃");
    temperature_sensor_handle_t tempSensor = NULL;
    temperature_sensor_config_t tempSensorConfig = {
        .range_min = 10,
        .range_max = 50,
    };
    ESP_ERROR_CHECK(temperature_sensor_install(&tempSensorConfig, &tempSensor));

    ESP_LOGI(TAG, "Enable temperature sensor");
    ESP_ERROR_CHECK(temperature_sensor_enable(tempSensor));

    ESP_LOGI(TAG, "Read temperature");
    int cnt = 20;
    float tsensValue;
    while (cnt--) {
        ESP_ERROR_CHECK(temperature_sensor_get_celsius(tempSensor, &tsensValue));
        ESP_LOGI(TAG, "Temperature value %.02f ℃", tsensValue);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
