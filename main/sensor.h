#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <stddef.h>

#define CONFIG_DHT11_PIN GPIO_NUM_26
#define CONFIG_HX711_DOUT GPIO_NUM_33
#define CONFIG_HX711_SCK GPIO_NUM_32
#define CONFIG_CONNECTION_TIMEOUT 5

typedef enum {
    SENSOR_TEMPERATURE,
    SENSOR_HUMIDITY,
    SENSOR_WEIGHT,
} sensor_type_t;

typedef struct {
    sensor_type_t type;
    float        value;
} sensor_reading_t;

typedef struct {
    sensor_reading_t *readings;
    size_t           count;
} sensor_payload_t;

void payload_to_json(const sensor_payload_t *p, char *out, size_t out_len);
void send_sensor_data(const sensor_payload_t *payload);

#endif
