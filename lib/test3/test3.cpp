#include <Arduino.h>
#include <SPIFFS.h>
#include "jpec.h"
#include "test3.h"

void setupTest3()
{
	Serial.println("____A____");
	File tmpFileBuffer;
	SPIFFS.remove("/tmp2.jpeg");

	tmpFileBuffer = SPIFFS.open("/tmp2.jpeg", FILE_WRITE);
	if (!tmpFileBuffer)
	{
		Serial.println("Failed to open file for writing");
	}

	Serial.println("____B____");
	uint16_t w = 300;
	uint16_t h = 200;
	uint8_t *img = (uint8_t *)malloc(w * h * sizeof(uint8_t));

	// create a picture
	unsigned x, y;
	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			img[1 * w * y + 1 * x + 0] = 255 * !(x & y);
		}
	}

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