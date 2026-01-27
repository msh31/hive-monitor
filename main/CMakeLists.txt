idf_component_register(SRCS "main.c" "wifi.c"
                       PRIV_REQUIRES spi_flash esp_wifi nvs_flash esp_netif esp_event esp_http_client esp32-dht11 
                       INCLUDE_DIRS "")
