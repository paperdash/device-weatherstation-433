#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include "time.h"

#define SENSOR_COUNT 20

void setupSensor();
void loopSensor();

typedef struct
{
    uint16_t id;
    float temperature;
    uint8_t humidity;
    time_t last_update;

    String protocol;
    String alias; // from user
} structSensorData;

structSensorData *getSensorList();

#endif