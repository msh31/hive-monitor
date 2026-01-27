#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h" // open-source real-0time OS on the ESP32
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "nvs_flash.h"
#include "esp_http_client.h"

#include "sensor_data.h"
#include "config.h"
#include "wifi.h"

#include <esp32-dht11.h>

#define CONFIG_DHT11_PIN GPIO_NUM_26
#define CONFIG_CONNECTION_TIMEOUT 5

static void payload_to_json(const sensor_payload_t *p, char *out, size_t out_len) {
    size_t pos = 0;
    pos += snprintf(out + pos, out_len - pos, "{");

    for (size_t i = 0; i < p->count; ++i) {
        const sensor_reading_t *r = &p->readings[i];
        const char *key = (r->type == SENSOR_TEMPERATURE) ? "temperature"
                         : (r->type == SENSOR_HUMIDITY)    ? "humidity"
                         : "unknown";

        if (i > 0) {
            pos += snprintf(out + pos, out_len - pos, ",");
        }

        pos += snprintf(out + pos, out_len - pos, "\"%s\":%.2f", key, r->value);
    }

    snprintf(out + pos, out_len - pos, "}");
}

void send_sensor_data(const sensor_payload_t *payload)
{
    char json_payload[256];
    payload_to_json(payload, json_payload, sizeof(json_payload));

    esp_http_client_config_t cfg = {
        .url    = API_URL,
        .method = HTTP_METHOD_PATCH,
    };

    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, json_payload, strlen(json_payload));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI("HTTP", "Status = %d", esp_http_client_get_status_code(client));
    } else {
        ESP_LOGE("HTTP", "Failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    create_wifi_connection(); //has basic error checking

    dht11_t dht11_sensor = { .dht11_pin = CONFIG_DHT11_PIN };

    // sensor_reading_t init_readings[] = {
    //     { .type = SENSOR_TEMPERATURE, .value = 42.0f },
    //     { .type = SENSOR_HUMIDITY,    .value = 69.0f }
    // };
    // sensor_payload_t init_payload = {
    //     .readings = init_readings,
    //     .count    = sizeof(init_readings) / sizeof(init_readings[0])
    // };
    // send_sensor_data(&init_payload); 

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
