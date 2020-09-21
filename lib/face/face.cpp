#include <Arduino.h>
#include <SPIFFS.h>
#include "jpec.h"
#include "face.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "datetime.h"
#include "sensor.h"

#include <Fonts/FreeMonoBold18pt7b.h>

#define GxEPD_BLACK 0x0000
#define GxEPD_WHITE 0xFFFF

GFXcanvas1 *face;

void imageGenerate(GFXcanvas1 *_canvas)
{
	Serial.println("imageGenerate...");

	// init
	_canvas->setRotation(0);
	_canvas->fillScreen(GxEPD_WHITE);
	_canvas->setTextColor(GxEPD_BLACK);
	_canvas->setTextSize(1);

	// init helper
	int16_t tbx, tby;
	uint16_t tbw, tbh, x;
	char label[64];

	// header - current time
	strftime(label, 64, "%c", &now);
	_canvas->setFont(&FreeMonoBold18pt7b);
	_canvas->getTextBounds(label, 0, 0, &tbx, &tby, &tbw, &tbh);
	x = (tbw / 2) - tbx;
	// Serial.println(x);
	_canvas->setCursor(10, 35);
	_canvas->print(label);

	// header - bottom border
	_canvas->fillRect(0, 50, 640, 15, GxEPD_BLACK);

	// sensor list
	structSensorData *list = getSensorList();
	size_t num = 0;

	for (uint8_t i = 0; i < SENSOR_COUNT; ++i)
	{
		if (strlen(list[i].label) > 0)
		{
			num++;
			_canvas->setCursor(20, 100 + (45 * num));
			_canvas->setTextColor(GxEPD_BLACK);
			_canvas->printf("% 5.1f C / %2d%%", list[i].temperature, list[i].humidity);

			_canvas->fillRect(305, 75 + (45 * num), 345, 35, GxEPD_BLACK);
			_canvas->setCursor(310, 100 + (45 * num));
			_canvas->setTextColor(GxEPD_WHITE);
			_canvas->printf("%s", list[i].label);
		}
	}
}

void setupFace()
{
	face = new GFXcanvas1(640, 384);
}

void updateFace()
{
	imageGenerate(face);
}

GFXcanvas1 *getFaceCanvas()
{
	return face;
}