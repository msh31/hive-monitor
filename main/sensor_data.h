#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <stddef.h>

typedef enum {
    SENSOR_TEMPERATURE,
    SENSOR_HUMIDITY,
} sensor_type_t;

typedef struct {
    sensor_type_t type;
    float        value;
} sensor_reading_t;

typedef struct {
    sensor_reading_t *readings;
    size_t           count;
} sensor_payload_t;

void process_sensor_payload(const sensor_payload_t *payload) {
    for (size_t i = 0; i < payload->count; ++i) {
        const sensor_reading_t *r = &payload->readings[i];

        switch (r->type) {
            case SENSOR_TEMPERATURE:
                printf("Temp: %.2f°C\n", r->value);
                break;
            case SENSOR_HUMIDITY:
                printf("Humidity: %.2f%%\n", r->value);
                break;
            default:
                printf("Unknown sensor %d = %.2f\n", r->type, r->value);
        }
    }
}

#endif
