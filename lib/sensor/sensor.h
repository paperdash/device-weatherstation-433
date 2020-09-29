#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include "time.h"

#define SENSOR_COUNT 20

void setupSensor();
void loopSensor();

typedef struct
{
	// sensor data
    uint16_t id;
    float temperature;
    uint8_t humidity;
    time_t last_update;
    char protocol[10];

	// user data
    char label[20];
} structSensorData;

structSensorData *getSensorList();

void updateSensor(uint16_t id, structSensorData sensor);
void deleteSensor(uint16_t id);

void saveSensors();

#endif