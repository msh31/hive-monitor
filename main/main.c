#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "esp_wifi_types_generic.h"
#include "freertos/FreeRTOS.h" // open-source real-0time OS on the ESP32
#include "freertos/task.h"

#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_netif.h"
#include "esp_event.h"
// #include "esp_http_client.h"

#include "wifi_config.h"
#include <esp32-dht11.h>

#define MAX_SSID_LENGTH 32
#define MAX_PASS_LENGTH 64
#define CONFIG_DHT11_PIN GPIO_NUM_26
#define CONFIG_CONNECTION_TIMEOUT 5

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
            break;
        case IP_EVENT_STA_GOT_IP:
            printf("WiFi got IP ... \n\n");
            break;
        default:
            break;
    }
}

void wifi_connection() {
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

    dht11_t dht11_sensor;
    dht11_sensor.dht11_pin = CONFIG_DHT11_PIN;

    while(true) {
        if(!dht11_read(&dht11_sensor, CONFIG_CONNECTION_TIMEOUT)) {  
            printf("[TEMP]> %.2f \n",dht11_sensor.temperature);
            printf("[HUMID]> %.2f \n",dht11_sensor.humidity);
        }
        vTaskDelay(2000/portTICK_PERIOD_MS);
    } 

    printf("Wifi Initialized!\n");
}
