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
uint16_t postFile(const char *fileName, const char *servername, uint16_t port);

// bmp filldata
uint8_t bmpFilldata[] = {0x0, 0x23, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0};

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

uint16_t postFile(const char *fileName, const char *servername, uint16_t port)
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
		uint16_t respCode = client.peek();
		uint16_t thisByte;
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

size_t write8(uint8_t *buffer, uint8_t v)
{
	memset(buffer, uint8_t(v), sizeof(uint8_t));

	return sizeof(uint8_t);
}

size_t write16(uint8_t *buffer, uint16_t v)
{
	memset(buffer, uint8_t(v), sizeof(uint8_t));
	memset(buffer + sizeof(uint8_t), uint8_t(v >> 8), sizeof(uint8_t));

	return sizeof(uint16_t);
}

size_t write32(uint8_t *buffer, uint32_t v)
{
	memset(buffer, uint8_t(v), sizeof(uint8_t));
	memset(buffer + sizeof(uint8_t), uint8_t(v >> 8), sizeof(uint8_t));
	memset(buffer + sizeof(uint8_t) * 2, uint8_t(v >> 16), sizeof(uint8_t));
	memset(buffer + sizeof(uint8_t) * 3, uint8_t(v >> 24), sizeof(uint8_t));

	return sizeof(uint32_t);
}

int displaySnapshotBMPStream(uint8_t *buffer, size_t maxLen, size_t index)
{
	GFXcanvas1 *_canvas = getFaceCanvas();

	uint8_t *bitmap = _canvas->getBuffer();
	int16_t w = _canvas->width();
	int16_t h = _canvas->height();

	uint16_t depth = 1;
	uint32_t rowSizeCode = (w + 8 - depth) * depth / 8;

	// BMP rows are padded (if needed) to 4-byte boundary
	uint32_t rowSizeBMP = (w * depth / 8 + 3) & ~3;
	uint32_t headerSize = 40;
	uint32_t imageOffset = 62;
	uint32_t fileSize = imageOffset + h * rowSizeBMP;

	uint32_t pointer = 0;
	if (index == 0)
	{
		pointer += write16(buffer + pointer, 0x4D42);	   // BMP signature
		pointer += write32(buffer + pointer, fileSize);	   // fileSize
		pointer += write32(buffer + pointer, 0);		   // creator bytes
		pointer += write32(buffer + pointer, imageOffset); // image offset
		pointer += write32(buffer + pointer, headerSize);  // Header size
		pointer += write32(buffer + pointer, w);		   // image width
		pointer += write32(buffer + pointer, h);		   // image height
		pointer += write16(buffer + pointer, 1);		   // # planes
		pointer += write16(buffer + pointer, depth);	   // bits per pixel
		pointer += write32(buffer + pointer, 0);		   // format uncompressed

		uint32_t j = 0;
		for (uint32_t i = 34; i < imageOffset; i++)
		{
			pointer += write8(buffer + pointer, bmpFilldata[j++]);
		}

		return pointer;
	}
	else
	{
		// calculate resume point
		size_t row_from = (index - imageOffset) / rowSizeBMP;
		uint32_t rowidx = w * h / 8;
		rowidx -= rowSizeCode * row_from;
		uint32_t colidx_from = (index - imageOffset) - (row_from * rowSizeBMP);

		// export image
		for (uint16_t row = row_from; row < h; row++) // for each line
		{
			rowidx -= rowSizeCode;

			uint32_t colidx;
			for (colidx = colidx_from; colidx < rowSizeCode; colidx++)
			{
				uint8_t data = pgm_read_byte(&bitmap[rowidx + colidx]);
				pointer += write8(buffer + pointer, data);

				if (pointer == maxLen)
				{
					return pointer;
				}
			}

			// reset resume
			colidx_from = 0;
		}

		return pointer;
	}
}