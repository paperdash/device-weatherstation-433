#include "sensor.h"
#include <SPIFFS.h>
#include <ESPiLight.h>
#define ARDUINOJSON_DECODE_UNICODE 1
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include "app.h"

#define RECEIVER_PIN 4 // any intterupt able pin

structSensorData sensorList[SENSOR_COUNT];

ESPiLight rf(-1); // disable transmitter

// config valid sensor protocols
const char *filterProtocol[] = {"alecto_wsd17", "alecto_ws1700", "alecto_wx500", "auriol", "ninjablocks_weather", "tcm", "teknihall", "tfa", "tfa30", "tfa2017"};
const uint8_t filterProtocolCount = sizeof(filterProtocol) / sizeof(filterProtocol[0]);

// config sensor protocol parsing
StaticJsonDocument<200> filter;
const size_t capacity = JSON_OBJECT_SIZE(3) + 30;
DynamicJsonDocument doc(capacity);

// config sensor db
const char *jsonStore = "/sensors.json";
unsigned long lastSavedTimestampMS = 0;
int32_t autoSaveEverySeconds = 300 * 1000; // 5 MIN

// private methods
void rfCallback(const String &protocol, const String &message, int status, size_t repeats, const String &deviceID);
void loadSensors();
void saveSensors();
bool isAutoSavedRequired();
void unsavedSinceReset();

void setupSensor()
{
	// filter sensor values
	filter["temperature"] = true;
	filter["humidity"] = true;

	// load sensors
	loadSensors();

	rf.setCallback(rfCallback);
	rf.initReceiver(RECEIVER_PIN);
}

void loopSensor()
{
	rf.loop();
	delay(10);

	if (isAutoSavedRequired())
	{
		saveSensors();
	}

	// TODO clean up every x minutes
}

structSensorData *getSensorList()
{
	return sensorList;
}

void sensorCleanUp()
{
	// TODO delete dead sensors
}

void loadSensors()
{
	File file = SPIFFS.open(jsonStore);
	if (!file)
	{
		Serial.print(F("Failed to open file: "));
		Serial.println(jsonStore);

		return;
	}

	// Allocate a temporary JsonDocument
	const size_t capacity = JSON_ARRAY_SIZE(20) + 20 * JSON_OBJECT_SIZE(4) + 800;
	StaticJsonDocument<capacity> doc;

	// Deserialize the JSON document
	DeserializationError error = deserializeJson(doc, file);
	if (error)
	{
		Serial.println(F("Failed to sensor file"));
		Serial.println(error.c_str());

		return;
	}

	// Copy values from the JsonDocument to the Config
	size_t count = doc.size();
	for (size_t i = 0; i < count && i < SENSOR_COUNT; i++)
	{
		sensorList[i].id = doc[i]["id"];
		sensorList[i].temperature = doc[i]["temperature"];
		sensorList[i].humidity = doc[i]["humidity"];
		sensorList[i].last_update = doc[i]["last_update"];
		strlcpy(sensorList[i].protocol,
				doc[i]["protocol"] | "",
				sizeof(sensorList[i].protocol));
		strlcpy(sensorList[i].label,
				doc[i]["label"] | "",
				sizeof(sensorList[i].label));
	}

	file.close();

	unsavedSinceReset();
}

void saveSensors()
{
	Serial.println(F("saveSensors..."));

	// Delete existing file, otherwise the configuration is appended to the file
	SPIFFS.remove(jsonStore);

	// Open file for writing
	File file = SPIFFS.open(jsonStore, FILE_WRITE);
	if (!file)
	{
		Serial.print(F("Failed to open file: "));
		Serial.println(jsonStore);

		return;
	}

	// Allocate a temporary JsonDocument
	const size_t capacity = JSON_ARRAY_SIZE(20) + 20 * JSON_OBJECT_SIZE(4) + 800;
	StaticJsonDocument<capacity> doc;

	// Copy values
	for (size_t i = 0; i < SENSOR_COUNT; i++)
	{
		doc[i]["id"] = sensorList[i].id;
		doc[i]["temperature"] = sensorList[i].temperature;
		doc[i]["humidity"] = sensorList[i].humidity;
		doc[i]["last_update"] = sensorList[i].last_update;
		doc[i]["protocol"] = sensorList[i].protocol;
		doc[i]["label"] = sensorList[i].label;
	}

	// Serialize JSON to file
	if (serializeJson(doc, file) == 0)
	{
		Serial.println(F("Failed to write to file"));
	}

	// Close the file
	file.close();

	unsavedSinceReset();
}

void updateSensor(uint16_t id, structSensorData sensor)
{
	uint8_t index = 255;

	// search sensor
	for (uint8_t s = 0; s < SENSOR_COUNT; s++)
	{
		if (sensorList[s].id == id)
		{
			index = s;
			break;
		}
	}

	// add sensor if necessary
	if (index == 255)
	{
		for (uint8_t s = 0; s < SENSOR_COUNT; s++)
		{
			// search for free or legacy slot
			if (sensorList[s].id == 0)
			{
				Serial.print(F("add sensor..."));
				Serial.println(id);

				sensorList[s].id = id;
				break;
			}
		}
	}

	if (index == 255)
	{
		Serial.println(F("no free slot found !!"));
		return;
	}

	// update sensor
	Serial.print(F("update sensor... slot #"));
	Serial.println(index);

	if (sensor.id)
	{
		sensorList[index].id = sensor.id;
	}
	if (sensor.temperature)
	{
		sensorList[index].temperature = sensor.temperature;
	}
	if (sensor.humidity)
	{
		sensorList[index].humidity = sensor.humidity;
	}
	if (sensor.last_update)
	{
		sensorList[index].last_update = sensor.last_update;
	}
	if (strlen(sensor.label) > 0)
	{
		strlcpy(sensorList[index].label, sensor.label, sizeof(sensorList[index].label));
	}
}


void rfCallback(const String &protocol, const String &message, int status, size_t repeats, const String &deviceID)
{
	// check if message is valid and process it
	if (status == VALID)
	{
		// allow only defined protocols
		for (int i = 0; i < filterProtocolCount; i++)
		{
			if (!strcmp(filterProtocol[i], protocol.c_str()))
			{
				/*
				Serial.print("Valid message: [");
				Serial.print(protocol);
				Serial.print("] ");
				Serial.print(deviceID);
				Serial.print(" - ");
				Serial.print(message);
				Serial.println();
				*/

				// get sensor data
				doc.clear();
				deserializeJson(doc, message, DeserializationOption::Filter(filter));

				structSensorData sensor;
				sensor.id = deviceID.toInt();
				sensor.temperature = doc["temperature"];
				sensor.humidity = doc["humidity"];
				sensor.last_update = time(0);
				strlcpy(sensor.protocol, protocol.c_str(), sizeof(sensor.protocol));

				// validate sensor data
				if (sensor.temperature <= -30 || sensor.temperature >= 60 || sensor.humidity < 0 || sensor.humidity > 100)
				{
					// invalid range
					return;
				}

				updateSensor(deviceID.toInt(), sensor);
				sendDataWs(doc);
			}
		}
	}
}

bool isAutoSavedRequired()
{
	int32_t remaining = autoSaveEverySeconds - (millis() - lastSavedTimestampMS);
	return remaining <= 0;
}

void unsavedSinceReset()
{
	lastSavedTimestampMS = millis();
}