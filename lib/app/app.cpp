#include <FS.h>
#include <SPIFFS.h>
#include <Update.h>
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>
#include "app.h"
#include "ESPAsyncWebServer.h"
#include "settings.h"
#include "sensor.h"

AsyncWebServer server(80);

void setupSettingsGet();
void setupSettingsPost();
void setupSensorsGet();
void setupWifiScan();
void setupWifiConnect();
void setupApiUpdate();
void setupOTA();

//flag to use from web update to reboot the ESP
bool shouldReboot = false;

void setupApp()
{
	Serial.println("setup configure");

	// @see https://github.com/me-no-dev/ESPAsyncWebServer
	// @see https://arduinojson.org/v6/assistant/

	// serve static files
	server
		.serveStatic("/", SPIFFS, "/dist/")
		.setDefaultFile("index.html")
		//.setCacheControl("max-age=600")
		;
	server.serveStatic("/fs/", SPIFFS, "/");

	setupSettingsGet();
	setupSettingsPost();
	setupSensorsGet();
	setupWifiScan();
	setupWifiConnect();
	setupApiUpdate();
	setupOTA();

	server.onNotFound([](AsyncWebServerRequest *request) {
		request->send(404);
	});

	// TODO response
	server.on("/stats", HTTP_GET, [](AsyncWebServerRequest *request) {
		AsyncResponseStream *response = request->beginResponseStream("application/json");
		DynamicJsonDocument doc(668); // https://arduinojson.org/v6/assistant/

		doc["wifi"]["mac"] = WiFi.macAddress();
		doc["wifi"]["ssid"] = WiFi.SSID();
		doc["wifi"]["connected"] = WiFi.isConnected();
		doc["wifi"]["rssi"] = WiFi.RSSI();
		doc["wifi"]["ip"] = WiFi.localIP().toString();
		doc["wifi"]["channel"] = WiFi.channel();
		doc["wifi"]["dns"] = WiFi.dnsIP().toString();
		doc["wifi"]["gateway"] = WiFi.gatewayIP().toString();

		/*
		doc["device"]["id"] = DeviceId;
		doc["device"]["heap"] = ESP.getFreeHeap();
		doc["device"]["bootCycle"] = deviceGetBootCount();
		*/

		doc["device"]["fs"]["total"] = SPIFFS.totalBytes();
		doc["device"]["fs"]["used"] = SPIFFS.usedBytes();
		doc["device"]["fs"]["free"] = SPIFFS.totalBytes() - SPIFFS.usedBytes();
		doc["device"]["time"] = time(NULL);

		serializeJson(doc, *response);
		request->send(response);
	});

	// CORS
	DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
	//DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "content-type"));

	server.begin();

	Serial.println("setup configure - done");
}

void loopApp()
{
	if (shouldReboot)
	{
		Serial.println("Rebooting...");
		delay(100);
		ESP.restart();
	}
}

void setupSensorsGet()
{
	server.on("/api/sensors", HTTP_GET, [](AsyncWebServerRequest *request) {
		String json = "[";

		structSensorData *list = getSensorList();
		for (uint8_t i = 0; i < SENSOR_COUNT; ++i)
		{
			if (list[i].id > 0)
			{
				if (i)
				{
					json += ",";
				}

				json += "{";
				json += "\"id\":" + String(list[i].id);
				json += ",\"temperature\":" + String(list[i].temperature);
				json += ",\"humidity\":" + String(list[i].humidity);
				json += ",\"last_update\":" + String(list[i].last_update);
				json += "}";
			}
		}

		json += "]";
		request->send(200, "application/json", json);
		json = String();
	});
}

void setupSettingsGet()
{
	server.on("/api/settings", HTTP_GET, [](AsyncWebServerRequest *request) {
		AsyncResponseStream *response = request->beginResponseStream("application/json");
		DynamicJsonDocument root(1024);

		root["system"]["country"] = NVS.getString("system.country");
		root["system"]["language"] = NVS.getString("system.language");
		root["system"]["timezone"] = NVS.getString("system.timezone");
		root["system"]["utc"] = NVS.getInt("system.utc");
		root["system"]["dst"] = NVS.getInt("system.dst");
		root["system"]["wifi"] = NVS.getString("wifi.ssid");

		serializeJson(root, *response);
		request->send(response);
	});
}

void setupSettingsPost()
{
	server.on(
		"/api/settings", HTTP_PUT, [](AsyncWebServerRequest *request) { /* nothing and dont remove it */ }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
		DynamicJsonDocument doc(2048);

		DeserializationError error = deserializeJson(doc, data);
		if (error) {
			Serial.print(F("deserializeJson() failed with code "));
			Serial.println(error.c_str());

			request->send(404, "text/plain", "");
		}
		else
		{

			// TODO

			NVS.commit();

			request->send(200, "application/ld+json; charset=utf-8", "{}");
		} });
}

/**
 * scan for wifi
 */
void setupWifiScan()
{
	server.on("/api/wifi/scan", HTTP_GET, [](AsyncWebServerRequest *request) {
		String json = "[";

		int n = WiFi.scanNetworks();
		for (int i = 0; i < n; ++i)
		{
			if (i)
			{
				json += ",";
			}

			json += "{";
			json += "\"rssi\":" + String(WiFi.RSSI(i));
			json += ",\"ssid\":\"" + WiFi.SSID(i) + "\"";
			json += ",\"bssid\":\"" + WiFi.BSSIDstr(i) + "\"";
			json += ",\"channel\":" + String(WiFi.channel(i));
			json += ",\"secure\":" + String(WiFi.encryptionType(i));
			json += "}";
		}

		WiFi.scanDelete();

		json += "]";
		request->send(200, "application/json", json);
		json = String();
	});
}

/**
 * @todo
 */
void setupWifiConnect()
{
	server.on(
		"/api/wifi/connect", HTTP_POST, [](AsyncWebServerRequest *request) { /* nothing and dont remove it */ }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
		DynamicJsonDocument doc(1024);

		DeserializationError error = deserializeJson(doc, data);
		if (error) {
			Serial.print(F("deserializeJson() failed with code "));
			Serial.println(error.c_str());

			request->send(404, "application/ld+json; charset=utf-8", "{}");
		}
		else
		{
			JsonVariant ssid = doc["ssid"];
			if (!ssid.isNull()) {
				NVS.setString("wifi.ssid", ssid);
				Serial.println(ssid.as<char*>());
			}

			JsonVariant password = doc["password"];
			if (!password.isNull()) {
				NVS.setString("wifi.password", password);
				Serial.println(password.as<char*>());
			}

/*
			if (doc.containsKey("ssid")) {
				NVS.setString("wifi_ssid", doc["ssid"]);
				Serial.println(doc["ssid"].as<char*>());
			}
			if (doc.containsKey("password")) {
				NVS.setString("wifi_password", doc["password"]);
				Serial.println(doc["password"].as<char*>());
			}
			*/

			request->send(200, "application/ld+json; charset=utf-8", "{}");

			ESP.restart();
		} });
}

void setupApiUpdate()
{
	server.on("/api/update", HTTP_GET, [](AsyncWebServerRequest *request) {
		if (request->hasParam("datetime"))
		{
			Serial.println("update datetime...");

			// TODO updateDateTime();
		}

		request->send(200, "application/ld+json; charset=utf-8", "{}");
	});
}

void setupOTA()
{
	// Simple Firmware Update Form
	/*
	server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request) {
		// TODO in die pwa auslagern
		request->send(200, "text/html", "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>");
	});
	*/

	server.on(
		"/update", HTTP_POST, [](AsyncWebServerRequest *request) {
			shouldReboot = !Update.hasError();
			AsyncWebServerResponse *response = request->beginResponse(200, "application/ld+json; charset=utf-8", shouldReboot ? "{\"success\": true}" : "{\"success\": false}");

			response->addHeader("Connection", "close");
			request->send(response); },
		[](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
			if (!index)
			{
				Serial.printf("Update Start: %s\n", filename.c_str());
				// bool canBegin = Update.begin(contentLength, U_FLASH);
				// bool canBegin = Update.begin(contentLength, U_SPIFFS);
				if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000))
				{
					Update.printError(Serial);
				}
			}

			if (!Update.hasError())
			{
				if (Update.write(data, len) != len)
				{
					Update.printError(Serial);
				}
			}

			if (final)
			{
				if (Update.end(true))
				{
					Serial.printf("Update Success: %uB\n", index + len);
				}
				else
				{
					Update.printError(Serial);
				}
			}
		});
}