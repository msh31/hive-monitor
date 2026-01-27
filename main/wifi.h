#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include "esp_event.h"

#define MAX_SSID_LENGTH 32
#define MAX_PASS_LENGTH 64

extern EventGroupHandle_t wifi_event_group;
extern const int WIFI_CONNECTED_BIT;

void create_wifi_connection(void);

#endif
