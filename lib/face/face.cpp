#include <Arduino.h>
#include <SPIFFS.h>
#include "jpec.h"
#include "face.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "datetime.h"

#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMono12pt7b.h>	  // weekday - month year
#include <Fonts/FreeSans24pt7b.h>	  // current day
#include <Fonts/FreeSansBold24pt7b.h> // current day

#define GxEPD_BLACK 0x0000
#define GxEPD_WHITE 0xFFFF

GFXcanvas1 *face;


void imageGenerate__(GFXcanvas1 *_canvas)
{
	_canvas->setTextColor(GxEPD_WHITE);
	_canvas->setTextSize(1);
	_canvas->setRotation(0);

	// init
	int16_t sideWidth = 250;
	int16_t tbx, tby;
	uint16_t tbw, tbh, x;
	char label[64];

	// left side
	_canvas->fillRect(0, 0, sideWidth, _canvas->height(), GxEPD_BLACK);

	// time
	strftime(label, 64, "%c", &now);
	_canvas->setFont(&FreeMono12pt7b);
	_canvas->setCursor(200, 30);
	_canvas->println(label);

	// weekday
	strftime(label, 64, "%A", &now);
	_canvas->setFont(&FreeMono12pt7b);
	_canvas->getTextBounds(label, 0, 0, &tbx, &tby, &tbw, &tbh);
	x = ((sideWidth - tbw) / 2) - tbx;
	_canvas->setCursor(x, 30);
	_canvas->println(label);

	// today
	_canvas->setFont(&FreeSansBold24pt7b);
	_canvas->setTextSize(2);
	_canvas->getTextBounds("29", 0, 0, &tbx, &tby, &tbw, &tbh);
	x = ((sideWidth - tbw) / 2) - tbx;
	_canvas->setCursor(x, 120);
	_canvas->println(now.tm_mday);

	// month yearh
	strftime(label, 64, "%B %Y", &now);
	_canvas->setTextSize(1);
	_canvas->setFont(&FreeMono12pt7b);
	_canvas->getTextBounds(label, 0, 0, &tbx, &tby, &tbw, &tbh);
	x = ((sideWidth - tbw) / 2) - tbx;
	_canvas->setCursor(x, 150);
	_canvas->println(label);

	// weekday headline
	//_canvas->setFont(&FreeMonoBold9pt7b);
	_canvas->setCursor(20, 192);
	_canvas->println("Mo Tu We Th Fr Sa Su");

	_canvas->setCursor(20, 220);

	// skip week days from previous month
	uint8_t skip = (now.day_offset == 0 ? 7 : now.day_offset);
	for (uint8_t d = 1; d < skip; d++)
	{
		_canvas->print("   ");
	}

	for (uint8_t d = 1; d <= now.days_in_month; d++)
	{
		_canvas->printf("%2d ", d);

		if ((d + now.day_offset - 1) % 7 == 0)
		{
			// new week
			_canvas->println("");
			_canvas->setCursor(20, _canvas->getCursorY());
		}
	}

	// current weather
	_canvas->drawLine(15, 320, sideWidth - 15, 320, GxEPD_WHITE);

	// temperature
	_canvas->setFont(&FreeSans24pt7b);
	_canvas->setTextSize(1);
	_canvas->setCursor(150, 367);
	_canvas->println("13°");
}

void setupFace()
{
	face = new GFXcanvas1(640, 384);

	//imageGenerate__(face);
	//exportJPG(_canvas);
}

void updateFace()
{
	imageGenerate__(face);
}

GFXcanvas1 * getFaceCanvas()
{
	return face;
}