#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h" // open-source real-0time OS on the ESP32
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_wifi_types_generic.h"

#include "config.h"
#include <esp32-dht11.h>

#define MAX_SSID_LENGTH 32
#define MAX_PASS_LENGTH 64
#define CONFIG_DHT11_PIN GPIO_NUM_26
#define CONFIG_CONNECTION_TIMEOUT 5

static EventGroupHandle_t wifi_event_group;
const int WIFI_CONNECTED_BIT = BIT0;

void send_sensor_data(float temp, float humidity) {
    // char json_payload[128];
    //
    // snprintf(json_payload, sizeof(json_payload), 
    //          "{\"temperature\":%.2f,\"humidity\":%.2f}", 
    //          temp, humidity);

    esp_http_client_config_t config = {
        .url = "http://webhook.site/cb1096ff-481e-4dd4-aa47-feeef8191e45",
        .method = HTTP_METHOD_POST,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    const char *post_data = "{\"field1\":\"value1\"}";
    esp_http_client_set_url(client, "http://webhook.site/cb1096ff-481e-4dd4-aa47-feeef8191e45");
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    int err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI("INF", "HTTP POST Status = %d, content_length = %"PRId64,
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE("INF", "HTTP POST request failed: %s", esp_err_to_name(err));
    }
}

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
        case WIFI_EVENT_STA_START:
            printf("WiFi connecting ... \n");
            break;
        case WIFI_EVENT_STA_CONNECTED:
            printf("WiFi connected ... \n");
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            printf("WiFi lost connection ... \n");
            xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
            break;
        case IP_EVENT_STA_GOT_IP:
            printf("WiFi got IP ... \n\n");
            xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
            break;
        default:
            break;
    }
}

void wifi_connection() {
    wifi_event_group = xEventGroupCreate(); 
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation);

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);

    wifi_config_t wifi_cfg = {};

    strncpy((char*)wifi_cfg.sta.ssid, WIFI_SSID, MAX_SSID_LENGTH);
    strncpy((char*)wifi_cfg.sta.password, WIFI_PASS, MAX_PASS_LENGTH);

    esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg);

    esp_wifi_start();
    esp_wifi_connect();
}

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_connection();

    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);
    printf("WiFi ready, starting HTTP client...\n");

    dht11_t dht11_sensor;
    dht11_sensor.dht11_pin = CONFIG_DHT11_PIN;

    send_sensor_data(1.0, 69.0);

    while(true) {
        if(dht11_read(&dht11_sensor, CONFIG_CONNECTION_TIMEOUT) != -1) {  
            printf("[TEMP]> %.2f \n",dht11_sensor.temperature);
            printf("[HUMID]> %.2f \n",dht11_sensor.humidity);
        }
        vTaskDelay(4000/portTICK_PERIOD_MS);
    } 

    printf("Wifi Initialized!\n");
}
