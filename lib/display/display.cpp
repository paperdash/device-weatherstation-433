#include <SPIFFS.h>
#include "display.h"
#include "toojpeg.h"
//#include <SPI.h>
//#include <Adafruit_GFX.h>



// @see https://create.stephan-brumme.com/toojpeg/
// https://create.stephan-brumme.com/toojpeg/example.cpp

File tmpFileBuffer;

// write a single byte compressed by tooJpeg
void myOutput(unsigned char oneByte)
{
	// fputc(oneByte, myFileHandle);
	Serial.print(".");
	if (tmpFileBuffer)
	{
		//fputc(oneByte, tmpFileBuffer);
		tmpFileBuffer.write(oneByte);
	}
}

bool createFaceJPG()
{
	// 800x600 image
	const auto width = 800;
	const auto height = 600;
	// RGB: one byte each for red, green, blue
	const auto bytesPerPixel = 3;

	// allocate memory
	auto image = new unsigned char[width * height * bytesPerPixel];

	// create a nice color transition (replace with your code)
	for (auto y = 0; y < height; y++)
		for (auto x = 0; x < width; x++)
		{
			// memory location of current pixel
			auto offset = (y * width + x) * bytesPerPixel;

			// red and green fade from 0 to 255, blue is always 127
			image[offset] = 255 * x / width;
			image[offset + 1] = 255 * y / height;
			image[offset + 2] = 127;
		}

	// start JPEG compression
	// note: myOutput is the function defined in line 18, it saves the output in example.jpg
	// optional parameters:
	const bool isRGB = true;					   // true = RGB image, else false = grayscale
	const auto quality = 90;					   // compression quality: 0 = worst, 100 = best, 80 to 90 are most often used
	const bool downsample = false;				   // false = save as YCbCr444 JPEG (better quality), true = YCbCr420 (smaller file)
	const char *comment = "TooJpeg example image"; // arbitrary JPEG comment
	bool ok = TooJpeg::writeJpeg(myOutput, image, width, height, isRGB, quality, downsample, comment);

	delete[] image;

	return ok;
}

void setupDisplay()
{
	SPIFFS.remove("/tmp.jpeg");

	tmpFileBuffer = SPIFFS.open("/tmp.jpeg", FILE_WRITE);
	if (!tmpFileBuffer)
	{
		Serial.println("Failed to open file for writing");
	}

	//createFaceJPG();

/*
	GFXcanvas1 *_canvas;

	_canvas = new GFXcanvas1(640, 384);
	/*
	_canvas->fillScreen(BLACK);
	_canvas->setFont(&FreeMonoBold12pt7b);
	_canvas->setTextColor(WHITE, BLACK);
	_canvas->setTextSize(fontsize);
	_canvas->setCursor(x, y);
	_canvas->print(strg);

	//_canvas->getBuffer()
	// tft.drawBitmap(x, y, _canvas->getBuffer(), w, h, WHITE, BLACK);

	delete _canvas;
	*/
}

void loopDisplay()
{
	// TODO create and send face image to connected epd
	// every x minutes
}
