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
//

#define MAX_SSID_LENGTH 32
#define MAX_PASS_LENGTH 64

typedef struct {
    char ssid[MAX_SSID_LENGTH + 1]; //+1 for null terminator
    char password[MAX_PASS_LENGTH + 1];
} WiFiLogin;

//  USER MUST CREATE THE config.txt FILE themselves
//  AND FILL IT WITH THE RIGHT CREDENTIALS
int load_config(const char *filename, WiFiLogin *login) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open config file");
        return -1;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "SSID=", 5) == 0) {
            strncpy(login->ssid, line + 5, MAX_SSID_LENGTH);
            login->ssid[MAX_SSID_LENGTH] = '\0';
        } else if (strncmp(line, "PASSWORD=", 9) == 0) {
            strncpy(login->password, line + 9, MAX_PASS_LENGTH);
            login->password[MAX_PASS_LENGTH] = '\0';
        }
    }

    fclose(file);
    return 0;
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
            break;
        case IP_EVENT_STA_GOT_IP:
            printf("WiFi got IP ... \n\n");
            break;
        default:
            break;
    }
}

void on_data_recv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {

}

void wifi_connection() {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation);

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);

    WiFiLogin cool_wifi;

    if (load_config("config.txt", &cool_wifi) == 0) {
        wifi_config_t wifi_cfg = {};

        strncpy((char*)wifi_cfg.sta.ssid, cool_wifi.ssid, MAX_SSID_LENGTH);
        strncpy((char*)wifi_cfg.sta.password, cool_wifi.password, MAX_PASS_LENGTH);

        esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg);

        esp_wifi_start();
        esp_wifi_connect();
    }
}

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_connection(); //TODO: add error check

    ESP_ERROR_CHECK(esp_now_init()); //may not be needed for this proj
    // ESP_ERROR_CHECK(esp_http_client_open)

    esp_now_register_recv_cb(on_data_recv);

    printf("Wifi Initialized!\n");
}
