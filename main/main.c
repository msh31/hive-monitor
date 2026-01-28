#include <stdint.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h" // open-source real-0time OS on the ESP32
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "nvs_flash.h"

#include "config.h"
#include "wifi.h"
#include "sensor.h"

#include <esp32-dht11.h>

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    create_wifi_connection(); //has basic error checking
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);
    printf("WiFi ready!\n");

    dht11_t dht11_sensor = { .dht11_pin = CONFIG_DHT11_PIN };

    while (true) {
        if (dht11_read(&dht11_sensor, CONFIG_CONNECTION_TIMEOUT) != -1) {
            printf("[TEMP]> %.2f\n", dht11_sensor.temperature);
            printf("[HUMID]> %.2f\n", dht11_sensor.humidity);

            sensor_reading_t live[] = {
                { .type = SENSOR_TEMPERATURE, .value = dht11_sensor.temperature },
                { .type = SENSOR_HUMIDITY,    .value = dht11_sensor.humidity }
            };
            sensor_payload_t live_payload = { .readings = live,
                                              .count    = 2 };
            send_sensor_data(&live_payload);
        }
        vTaskDelay(60000 / portTICK_PERIOD_MS);
    }
}
