#include <Arduino.h>
#include <SPIFFS.h>
#include "wlan.h"
#include "settings.h"
#include "sensor.h"
#include "datetime.h"
#include "display.h"
#include "face.h"
#include "app.h"

void setup()
{
	Serial.begin(115200);

	delay(100); //Take some time to open up the Serial Monitor
	Serial.println();
	Serial.println("setup...");
	Serial.println();

	if (!SPIFFS.begin())
	{
		Serial.println("An Error has occurred while mounting SPIFFS");
		return;
	}

	setupSettings();
	setupSensor();

	setupWlan();
	if (WiFi.isConnected())
	{
		setupDateTime();
	}

	setupFace();
	setupDisplay();
	setupApp();

	Serial.println();
	Serial.println("setup - done");
}

void loop()
{
	if (WiFi.isConnected())
	{
		loopDateTime();
	}

	loopSensor();
	loopApp();
}