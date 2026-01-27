#include <stdio.h>
#include <string.h>

#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi_types_generic.h"

#include "wifi.h"
#include "config.h"

EventGroupHandle_t wifi_event_group;
const int WIFI_CONNECTED_BIT = BIT0;

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

void create_wifi_connection() {
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

    printf("Establishing connection with network: %s", wifi_cfg.sta.ssid);
    esp_wifi_start();
    esp_wifi_connect();

}
