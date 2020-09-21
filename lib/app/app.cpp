#include <FS.h>
#include <SPIFFS.h>
#include <Update.h>
#define ARDUINOJSON_DECODE_UNICODE 1
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include "app.h"
#include "settings.h"
#include "sensor.h"
//#include "face.h"
#include "display.h"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

void setupSettingsGet();
void setupSettingsPost();
void setupSensorsGet();
void setupSensorUpdatePut();
void setupEpdScan();
void setupEpdUpdate();
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
	setupSensorUpdatePut();
	setupEpdScan();
	setupEpdUpdate();
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

	ws.onEvent(onWsEvent);
	server.addHandler(&ws);

	events.onConnect([](AsyncEventSourceClient *client) {
		client->send("hello!", NULL, millis(), 1000);
	});
	server.addHandler(&events);

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

	ws.cleanupClients();
}

void setupSensorsGet()
{
	server.on("/api/sensors", HTTP_GET, [](AsyncWebServerRequest *request) {
		AsyncResponseStream *response = request->beginResponseStream("application/ld+json; charset=utf-8");

		// Allocate a temporary JsonDocument
		const size_t capacity = JSON_ARRAY_SIZE(20) + 20 * JSON_OBJECT_SIZE(4) + 800;
		StaticJsonDocument<capacity> root;

		structSensorData *list = getSensorList();
		for (uint8_t i = 0; i < SENSOR_COUNT; ++i)
		{
			if (list[i].id > 0)
			{
				root[i]["id"] = list[i].id;
				root[i]["temperature"] = list[i].temperature;
				root[i]["humidity"] = list[i].humidity;
				root[i]["last_update"] = list[i].last_update;
				root[i]["protocol"] = list[i].protocol;
				root[i]["label"] = list[i].label;
			}
		}

		serializeJson(root, *response);
		request->send(response);

		/*
		root["system"]["country"] = NVS.getString("system.country");
		root["system"]["language"] = NVS.getString("system.language");
		root["system"]["timezone"] = NVS.getString("system.timezone");
		root["system"]["utc"] = NVS.getInt("system.utc");
		root["system"]["dst"] = NVS.getInt("system.dst");
		root["system"]["wifi"] = NVS.getString("wifi.ssid");

		serializeJson(root, *response);
		request->send(response);

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
				json += ",\"temperature\":" + String(list[i].temperature || 0);
				json += ",\"humidity\":" + String(list[i].humidity || 0);
				json += ",\"last_update\":" + String(list[i].last_update);
				json += ",\"protocol\":\"" + String(list[i].protocol) + "\"";
				json += ",\"label\":\"" + String(list[i].label) + "\"";
				json += "}";
			}
		}

		json += "]";
		request->send(200, "application/json", json);
		json = String();
		*/
	});
}

void setupSensorUpdatePut()
{
	server.on(
		"^\\/api\\/sensor\\/([0-9]+)$", HTTP_PUT, [](AsyncWebServerRequest *request) { /* nothing and dont remove it */ }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
			Serial.println(F("update sensor from webserver"));

			uint16_t sensorId = request->pathArg(0).toInt();
			DynamicJsonDocument doc(2048);

			DeserializationError error = deserializeJson(doc, data);
			if (error)
			{
				Serial.print(F("deserializeJson() failed with code "));
				Serial.println(error.c_str());

				request->send(404, "text/plain", "");
			}
			else
			{
				structSensorData sensor;
				memset(&sensor, 0, sizeof(sensor));

				JsonVariant id = doc["id"];
				if (!id.isNull()) {
					sensor.id = id.as<uint16_t>();
				} else {
					sensor.id = sensorId;
				}


				JsonVariant label = doc["label"];
				if (!label.isNull()) {
					Serial.println(label.as<char*>());

					strlcpy(sensor.label,
							label.as<char*>(),
							sizeof(sensor.label));
				}

				updateSensor(sensorId, sensor);
				saveSensors();

				request->send(200, "application/ld+json; charset=utf-8", "{}");
			} });
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

void setupEpdScan()
{
	server.on("/api/epd/scan", HTTP_GET, [](AsyncWebServerRequest *request) {
		String json = "[";
		Serial.printf("Browsing for service _%s._%s.local. ... ", "http", "tcp");
		int n = MDNS.queryService("http", "tcp");
		if (n == 0)
		{
			Serial.println("no services found");
		}
		else
		{
			Serial.print(n);
			Serial.println(" service(s) found");

			size_t cnt = 0;
			for (size_t i = 0; i < n; ++i)
			{
				// checking for epd
				if (MDNS.hasTxt(i, "epd"))
				{
					if (cnt)
					{
						json += ",";
					}
					cnt++;

					json += "{";
					json += "\"hostname\":\"" + MDNS.hostname(i) + "\"";
					json += ",\"ip\":\"" + MDNS.IP(i).toString() + "\"";
					json += ",\"port\":" + String(MDNS.port(i));
					json += "}";
				}
			}
		}
		Serial.println();

		json += "]";
		request->send(200, "application/json", json);
		json = String();
	});
}

void setupEpdUpdate()
{
	server.on("/api/epd/update", HTTP_GET, [](AsyncWebServerRequest *request) {

		updateDisplay();

		request->send(200, "application/json", "{}");
	});
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

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
	if (type == WS_EVT_CONNECT)
	{
		Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
		// client->printf("Hello Client %u :)", client->id());
		client->ping();
	}
	else if (type == WS_EVT_DISCONNECT)
	{
		Serial.printf("ws[%s][%u] disconnect\n", server->url(), client->id());
	}
	else if (type == WS_EVT_ERROR)
	{
		Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t *)arg), (char *)data);
	}
	else if (type == WS_EVT_PONG)
	{
		Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char *)data : "");
	}
	else if (type == WS_EVT_DATA)
	{
		AwsFrameInfo *info = (AwsFrameInfo *)arg;
		String msg = "";
		if (info->final && info->index == 0 && info->len == len)
		{
			//the whole message is in a single frame and we got all of it's data
			Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);

			if (info->opcode == WS_TEXT)
			{
				for (size_t i = 0; i < info->len; i++)
				{
					msg += (char)data[i];
				}
			}
			else
			{
				char buff[3];
				for (size_t i = 0; i < info->len; i++)
				{
					sprintf(buff, "%02x ", (uint8_t)data[i]);
					msg += buff;
				}
			}
			Serial.printf("%s\n", msg.c_str());

			if (info->opcode == WS_TEXT)
				client->text("I got your text message");
			else
				client->binary("I got your binary message");
		}
		else
		{
			//message is comprised of multiple frames or the frame is split into multiple packets
			if (info->index == 0)
			{
				if (info->num == 0)
					Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
				Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
			}

			Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT) ? "text" : "binary", info->index, info->index + len);

			if (info->opcode == WS_TEXT)
			{
				for (size_t i = 0; i < len; i++)
				{
					msg += (char)data[i];
				}
			}
			else
			{
				char buff[3];
				for (size_t i = 0; i < len; i++)
				{
					sprintf(buff, "%02x ", (uint8_t)data[i]);
					msg += buff;
				}
			}
			Serial.printf("%s\n", msg.c_str());

			if ((info->index + len) == info->len)
			{
				Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
				if (info->final)
				{
					Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
					if (info->message_opcode == WS_TEXT)
						client->text("I got your text message");
					else
						client->binary("I got your binary message");
				}
			}
		}
	}
}

void sendDataWs(DynamicJsonDocument doc)
{
	// Serial.println(F("sendDataWs"));

	size_t len = measureJson(doc);
	AsyncWebSocketMessageBuffer *buffer = ws.makeBuffer(len); //  creates a buffer (len + 1) for you.
	if (buffer)
	{
		serializeJson(doc, (char *)buffer->get(), len + 1);
		ws.textAll(buffer);
	}
}