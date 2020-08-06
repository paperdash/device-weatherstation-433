#include "wlan.h"
#include <ArduinoNvs.h>

const char *deviceName = "paperdash-weatherstation";
RTC_DATA_ATTR int wifiFailedCount = 0;

void initClientMode(const char *ssid, const char *password);
void initAPMode();

void setupWlan()
{
	Serial.println("setup Wlan");

	// load wifi settings
	String ssid = NVS.getString("wifi.ssid");
	String password = NVS.getString("wifi.password");

	// TODO count failed connecting wifiFailedCount <=3
	if (!ssid.isEmpty() && !password.isEmpty() && wifiFailedCount <=3) // && wifiFailedCount <=3
	{
		// client mode
		initClientMode(ssid.c_str(), password.c_str());
	}
	else
	{
		// ap mode
		initAPMode();
	}

	WiFi.setHostname(deviceName);

	Serial.println("setup Wlan - done");
}

void initClientMode(const char *ssid, const char *password)
{
	uint8_t tryCount = 5;
	long startMills = millis();

	Serial.print("  Connecting to: ");
	Serial.print(ssid);
	Serial.print(" ");

	// connecting
	WiFi.mode(WIFI_STA);


	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
		if (!tryCount--)
		{
			// todo, hier passt was nicht
			wifiFailedCount++;
			if (wifiFailedCount > 3) {
				Serial.println("  wifi is not reachable...");
				initAPMode();
				return;
			} else {
				tryCount = 5;
				WiFi.disconnect();
				Serial.println("  wifi reset...");
				delay(500);
				WiFi.begin(ssid, password);
			}
		}
	}

	Serial.println(" ...connected");
	Serial.print("  IP address: ");
	Serial.println(WiFi.localIP());

    if (!MDNS.begin(deviceName)) {
        Serial.println("Error setting up MDNS responder!");
    }

	Serial.print("  connected in: ");
	Serial.println(millis() - startMills);
}

void initAPMode()
{
	Serial.println("  init AP (Access Point)");

	WiFi.softAP("paperdash.io");

	IPAddress IP = WiFi.softAPIP();
	Serial.print("  AP IP address: ");
	Serial.println(IP);
}