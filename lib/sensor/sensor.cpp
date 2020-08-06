#include "sensor.h"
#include <ESPiLight.h>
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>

#define RECEIVER_PIN 4 // any intterupt able pin

structSensorData sensorList[SENSOR_COUNT];

ESPiLight rf(-1); // disable transmitter

const char *filterProtocol[] = {"alecto_wsd17", "alecto_ws1700", "alecto_wx500", "auriol", "ninjablocks_weather", "tcm", "teknihall", "tfa", "tfa30", "tfa2017"};
const uint8_t filterProtocolCount = sizeof(filterProtocol) / sizeof(filterProtocol[0]);

StaticJsonDocument<200> filter;

const size_t capacity = JSON_OBJECT_SIZE(3) + 30;
DynamicJsonDocument doc(capacity);

// callback function. It is called on successfully received and parsed rc signal
void rfCallback(const String &protocol, const String &message, int status,
                size_t repeats, const String &deviceID)
{
  // check if message is valid and process it
  if (status == VALID)
  {
    // allow only defined protocols
    for (int i = 0; i < filterProtocolCount; i++)
    {
      if (!strcmp(filterProtocol[i], protocol.c_str()))
      {
        Serial.print("Valid message: [");
        Serial.print(protocol);
        Serial.print("] ");
        Serial.print(deviceID);
        Serial.print(" - ");
        Serial.print(message);
        Serial.println();

        // get sensor data
        doc.clear();
        deserializeJson(doc, message, DeserializationOption::Filter(filter));
        float temperature = doc["temperature"];
        int8_t humidity = doc["humidity"];

        // validate sensor data
        if (temperature <= -30 || temperature >= 60 || humidity < 0 || humidity > 100)
        {
          // invalid range
          return;
        }

        // update existing sensor
        for (uint8_t s = 0; s < SENSOR_COUNT; s++)
        {
          if (sensorList[s].id == deviceID.toInt())
          {
            Serial.println(F("update sensor..."));

            sensorList[s].temperature = temperature;
            sensorList[s].humidity = humidity;
            sensorList[s].last_update = time(0);
            return;
          }
        }

        // add sensor
        for (uint8_t s = 0; s < SENSOR_COUNT; s++)
        {
          if (sensorList[s].id == 0)
          {
            Serial.println(F("add sensor..."));

            sensorList[s].id = deviceID.toInt();
            sensorList[s].temperature = temperature;
            sensorList[s].humidity = humidity;
            sensorList[s].last_update = time(0);
            return;
          }
        }
      }
    }
  }
}

void setupSensor()
{
  // filter values
  filter["temperature"] = true;
  filter["humidity"] = true;

  rf.setCallback(rfCallback);
  rf.initReceiver(RECEIVER_PIN);
}

void loopSensor()
{
  rf.loop();
  delay(10);

  // TODO clean up every x minutes
}

structSensorData *getSensorList()
{
  return sensorList;
}


void sensorCleanUp() {
  // TODO delete dead sensors
}