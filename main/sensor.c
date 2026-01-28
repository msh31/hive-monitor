#include <stddef.h>
#include <string.h>

#include "sensor.h"
#include "config.h"

#include "esp_log.h"
#include "esp_http_client.h"

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
