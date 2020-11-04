#include <WiFiClient.h>
#include <SPIFFS.h>
#include <Adafruit_GFX.h>
#include <ArduinoNvs.h>
#include "display.h"
#include "face.h"
#include "jpec.h"

#define MTU_Size 2 * 1460 // this size seems to work best
WiFiClient client;
File tmpFileCache;

unsigned long lastSwitch = 0;
int32_t timer;

int32_t displayGetRemainingTimeMs();
void exportJPG(GFXcanvas1 *_canvas, const char *fileName);
byte postFile(const char *fileName, const char *servername, uint16_t port);

void setupDisplay()
{
	Serial.println("setupDisplay...");

	// TODO load timer
	/*
	timer = NVS.getInt("display.timer") * 1000;
	if (timer < 30000)
	{
		timer = 30000;
	}
	*/
	timer = 300 * 1000; // 5 min

	// force instant update
	lastSwitch = millis() - timer;
}

void loopDisplay()
{
	if (displayGetRemainingTimeMs() <= 0)
	{
		lastSwitch = millis();
		updateDisplay();
	}
}

bool updateDisplay()
{
	String host = NVS.getString("display.host");

	if (!host.isEmpty())
	{
		updateDisplay(host.c_str());
	}

	return true;
}

bool updateDisplay(const char *host)
{
	Serial.printf("Update display '%s'\n", host);

	updateFace();
	GFXcanvas1 *_canvas = getFaceCanvas();

	exportJPG(_canvas, "/tmp2.jpeg");
	postFile("/tmp2.jpeg", host, 80);

	return true;
}

int32_t displayGetRemainingTimeMs()
{
	return timer - (millis() - lastSwitch);
}

void exportJPG(GFXcanvas1 *_canvas, const char *fileName)
{
	SPIFFS.remove(fileName);
	tmpFileCache = SPIFFS.open(fileName, FILE_WRITE);
	if (!tmpFileCache)
	{
		Serial.println("Failed to open file for writing");
	}

	/* Create a JPEG encoder provided image data */
	jpec_enc_t *e = jpec_enc_new2(_canvas->getBuffer(), _canvas->width(), _canvas->height(), 50, [](int offset, uint8_t val) {
		tmpFileCache.write(val);
	});

	/* Compress */
	int len;
	long startMills = millis();
	jpec_enc_run(e, &len);
	Serial.print(millis() - startMills);
	Serial.println("ms");

	/* Release the encoder */
	jpec_enc_del(e);
	tmpFileCache.close();
}

byte postFile(const char *fileName, const char *servername, uint16_t port)
{
	// connect
	client.setTimeout(2);
	if (client.connect(servername, port))
	{
		File fp = SPIFFS.open(fileName, FILE_READ);

		client.setNoDelay(true);
		Serial.println("Connected to Server");
		Serial.println("");

		// Make a HTTP request:
		client.println("POST /api/face HTTP/1.0");
		client.println("Host: 192.168.178.65");
		client.println("User-Agent: curl/7.64.1");
		client.println("Accept: */*");
		client.printf("Content-Length: %d\n", fp.size() + 174);
		client.println("Content-Type: multipart/form-data; boundary=------------------------5c8e39ca1848977d");
		client.println("");

		client.println("--------------------------5c8e39ca1848977d");
		client.println("Content-Disposition: form-data; filename=\"dithering\"");
		client.println("Content-Type: image/jpeg");
		client.println("");

		byte clientBuf[MTU_Size];
		int clientCount = 0;
		while (fp.available())
		{
			clientCount = fp.available() >= MTU_Size ? MTU_Size : fp.available();
			fp.read(&clientBuf[0], clientCount);

			if (clientCount > 0)
			{
				client.write((const uint8_t *)&clientBuf[0], clientCount);
			}
		}

		Serial.println();
		client.println();

		client.println("--------------------------5c8e39ca1848977d--");
		fp.close();

		// wait for response
		while (!client.available())
		{
			delay(1);
		}

		// read response
		byte respCode = client.peek();
		byte thisByte;
		while (client.available())
		{
			thisByte = client.read();
			Serial.write(thisByte);
		}

		client.stop();

		return respCode;
	}

	return 404;
}