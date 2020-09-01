#include <Arduino.h>
#include <SPIFFS.h>
#include "jpec.h"
#include "test3.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeMonoBold12pt7b.h>

#define GxEPD_BLACK 0x0000
#define GxEPD_WHITE 0xFFFF

// bmp
void write16(File &f, uint16_t v);
void write32(File &f, uint32_t v);
uint8_t filldata2[] = {0x0, 0x23, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0};
// bmp

void imageTest3()
{
	uint16_t w = 10;
	uint16_t h = 10;
	// draw x / y line, 10 px each


	// test original version
	uint8_t *img = (uint8_t *)calloc(w * h, sizeof(uint8_t));
	img[w * 0 + 0] = 255;
	img[w * 0 + 1] = 255;
	img[w * 0 + 2] = 255;
	img[w * 0 + 3] = 255;
	img[w * 0 + 4] = 255;
	img[w * 0 + 5] = 255;
	img[w * 0 + 6] = 255;
	img[w * 0 + 7] = 255;
	img[w * 0 + 8] = 255;
	img[w * 0 + 9] = 255;
	img[w * 1 + 0] = 255;
	img[w * 2 + 0] = 255;
	img[w * 3 + 0] = 255;
	img[w * 4 + 0] = 255;
	img[w * 5 + 0] = 255;
	img[w * 6 + 0] = 255;
	img[w * 7 + 0] = 255;
	img[w * 8 + 0] = 255;
	img[w * 9 + 0] = 255;


	// test new version
	GFXcanvas1 *_canvas;
	_canvas = new GFXcanvas1(w, h);
	_canvas->fillScreen(GxEPD_BLACK);
	_canvas->drawPixel(0, 0, GxEPD_WHITE);
	_canvas->drawPixel(2, 0, GxEPD_WHITE);
	_canvas->drawPixel(6, 0, GxEPD_WHITE);
	//_canvas->drawPixel(10, 0, GxEPD_WHITE);
	//_canvas->drawPixel(1, 10, GxEPD_WHITE);
	//_canvas->drawLine(0, 0, 0, 10, GxEPD_WHITE);
	//_canvas->drawLine(0, 0, 10, 0, GxEPD_WHITE);
	uint8_t *buffer = _canvas->getBuffer();
	//uint8_t a = _canvas->getPixel(0,0);

	#define JPEC_BLOCK_old(col,row) img[row * w + col]
	#define JPEC_BLOCK_new(col,row) ((buffer[(col / 8) + row * ((w + 7) / 8)]) & (0x80 >> (col & 7))) != 0 ? 255.0 : 0.0


	for (size_t y = 0; y < 2; y++) {
		for (size_t x = 0; x < w; x++) {
			float color_old = (float) JPEC_BLOCK_old(x, y);
			uint8_t color_new = JPEC_BLOCK_new(x, y);

			Serial.printf("y: %d, x: %d | ", y, x);
			Serial.print(color_old);
			Serial.print(" | new: ");
			Serial.println(color_new);
		}
	}
}

// framebuffer verwenden
void imageTest2()
{
	GFXcanvas1 *_canvas;
	_canvas = new GFXcanvas1(640, 384);
	_canvas->fillScreen(GxEPD_BLACK);
	_canvas->setFont(&FreeMonoBold12pt7b);
	_canvas->setTextColor(GxEPD_WHITE, GxEPD_BLACK);
	_canvas->setTextSize(3);
	_canvas->setCursor(20, 50);

	_canvas->print("hallo paperdash");
	_canvas->fillRect(0, 0, 20, 2, GxEPD_WHITE);
	_canvas->fillRect(40, 0, 30, 2, GxEPD_WHITE);

	_canvas->fillRect(0, 0, 2, 40, GxEPD_WHITE);
	_canvas->fillRect(0, 70, 10, 10, GxEPD_WHITE);

	_canvas->fillRect(100, 100, 40, 40, GxEPD_WHITE);
	_canvas->fillRect(300, 200, 100, 100, GxEPD_WHITE);
	_canvas->fillRect(600, 340, 100, 100, GxEPD_WHITE);

	_canvas->drawLine(0, 0, 0, 10, GxEPD_WHITE);
	_canvas->drawLine(0, 0, 10, 0, GxEPD_WHITE);

	Serial.println("____C____");

	/* Create a JPEG encoder provided image data */
	jpec_enc_t *e = jpec_enc_new2(_canvas->getBuffer(), _canvas->width(), _canvas->height(), 50);
	// jpec_enc_t *e = jpec_enc_new2(img, w, h, 70);
	Serial.println("____D____");

	/* Compress */
	int len;
	long startMills = millis();
	jpec_enc_run(e, &len);
	Serial.println(millis() - startMills);
	Serial.println("____E____");

	/* Do something with the JPEG blob (e.g. save into a file, etc) */
	// tmpFileBuffer.write(jpeg, len);

	// printf("Done: result.jpg (%d bytes)\n", len);
	Serial.println("____F____");
	/* Release the encoder */
	//jpec_enc_del(e);
	Serial.println("____G____");

	File tmpFileBuffer;
	SPIFFS.remove("/tmp2.bmp");
	tmpFileBuffer = SPIFFS.open("/tmp2.bmp", FILE_WRITE);
	if (!tmpFileBuffer)
	{
		Serial.println("Failed to open file for writing");
	}

	uint8_t *bitmap = _canvas->getBuffer();
	int16_t w = _canvas->width();
	int16_t h = _canvas->height();

	uint16_t depth = 1;
	uint32_t rowSizeCode = (w + 8 - depth) * depth / 8;
	Serial.print("rowSizeCode: ");
	Serial.println(rowSizeCode);

	// BMP rows are padded (if needed) to 4-byte boundary
	uint32_t rowSizeBMP = (w * depth / 8 + 3) & ~3;
	uint32_t headerSize = 40;
	uint32_t imageOffset = 62;
	uint32_t fileSize = imageOffset + h * rowSizeBMP;

	write16(tmpFileBuffer, 0x4D42);		 // BMP signature
	write32(tmpFileBuffer, fileSize);	 // fileSize
	write32(tmpFileBuffer, 0);			 // creator bytes
	write32(tmpFileBuffer, imageOffset); // image offset
	write32(tmpFileBuffer, headerSize);	 // Header size
	write32(tmpFileBuffer, w);			 // image width
	write32(tmpFileBuffer, h);			 // image height
	write16(tmpFileBuffer, 1);			 // # planes
	write16(tmpFileBuffer, depth);		 // bits per pixel
	write32(tmpFileBuffer, 0);			 // format uncompressed

	uint32_t j = 0;
	for (uint32_t i = 34; i < imageOffset; i++)
	{
		tmpFileBuffer.write(filldata2[j++]); // remaining header bytes
	}

	uint32_t rowidx = w * h / 8;
	for (uint16_t row = 0; row < h; row++) // for each line
	{
		rowidx -= rowSizeCode;

		uint32_t colidx;
		for (colidx = 0; colidx < rowSizeCode; colidx++)
		{
			uint8_t data = pgm_read_byte(&bitmap[rowidx + colidx]);
			tmpFileBuffer.write(data);

			//uint8_t data = _canvas->getPixel(colidx, row);
			//tmpFileBuffer.write(data);
		}

		while (colidx++ < rowSizeBMP)
		{
			tmpFileBuffer.write(uint8_t(0)); // padding
		}
	}

	tmpFileBuffer.close();
}

void imageTest1()
{
	Serial.println("____A____");
	File tmpFileBuffer;
	SPIFFS.remove("/tmp1.jpeg");

	tmpFileBuffer = SPIFFS.open("/tmp1.jpeg", FILE_WRITE);
	if (!tmpFileBuffer)
	{
		Serial.println("Failed to open file for writing");
	}

	Serial.println("____B____");
	uint16_t w = 40;
	uint16_t h = 40;
	uint8_t *img = (uint8_t *)calloc(w * h, sizeof(uint8_t));

	// create a picture
	/*
	unsigned x, y;
	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			img[(w * y) + x] = 255 * !(x & y);
			img[(w * y) + x] = 0;
		}
	}
	*/

	img[w * 0 + 0] = 255;
	img[w * 0 + 1] = 255;
	img[w * 0 + 2] = 255;
	img[w * 0 + 3] = 255;
	img[w * 0 + 4] = 255;
	img[w * 0 + 5] = 255;
	img[w * 0 + 6] = 255;
	img[w * 0 + 7] = 255;
	img[w * 0 + 8] = 255;
	img[w * 0 + 9] = 255;
	img[w * 1 + 0] = 255;
	img[w * 2 + 0] = 255;
	img[w * 3 + 0] = 255;
	img[w * 4 + 0] = 255;
	img[w * 5 + 0] = 255;
	img[w * 6 + 0] = 255;
	img[w * 7 + 0] = 255;
	img[w * 8 + 0] = 255;
	img[w * 9 + 0] = 255;

	Serial.println("____C____");

	/* Create a JPEG encoder provided image data */
	jpec_enc_t *e = jpec_enc_new2(img, w, h, 70);
	Serial.println("____D____");

	/* Compress */
	int len;
	const uint8_t *jpeg = jpec_enc_run(e, &len);
	Serial.println("____E____");

	/* Do something with the JPEG blob (e.g. save into a file, etc) */
	tmpFileBuffer.write(jpeg, len);

	// printf("Done: result.jpg (%d bytes)\n", len);
	Serial.println("____F____");
	/* Release the encoder */
	jpec_enc_del(e);
	Serial.println("____G____");
}

void setupTest3()
{
	imageTest2();
}

void write16(File &f, uint16_t v)
{
	f.write(uint8_t(v));
	f.write(uint8_t(v >> 8));
}

void write32(File &f, uint32_t v)
{
	f.write(uint8_t(v));
	f.write(uint8_t(v >> 8));
	f.write(uint8_t(v >> 16));
	f.write(uint8_t(v >> 24));
}