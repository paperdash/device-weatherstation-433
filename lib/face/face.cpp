#include <lodepng.h>
#include <stdio.h>
#include <stdlib.h>
#include "face.h"

void setupFace()
{

}



void encodeTwoSteps(const char* filename, const unsigned char* image, unsigned width, unsigned height);
void exportImage();

void setupFace()
{

}

void loopFace()
{

}


/*
Example 2
Encode from raw pixels to an in-memory PNG file first, then write it to disk
The image argument has width * height RGBA pixels or width * height * 4 bytes
*/

void encodeTwoSteps(const char* filename, const unsigned char* image, unsigned width, unsigned height) {
  unsigned char* png;
  size_t pngsize;

  unsigned error = lodepng_encode32(&png, &pngsize, image, width, height);
  if(!error) lodepng_save_file(png, pngsize, filename);

  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

  free(png);
}

void exportImage() {
  const char* filename = "test.png";

  unsigned width = 512, height = 512;
  unsigned char* image = (unsigned char*)malloc(width * height * 4);
  unsigned x, y;
  for(y = 0; y < height; y++)
  for(x = 0; x < width; x++) {
    image[4 * width * y + 4 * x + 0] = 255 * !(x & y);
    image[4 * width * y + 4 * x + 1] = x ^ y;
    image[4 * width * y + 4 * x + 2] = x | y;
    image[4 * width * y + 4 * x + 3] = 255;
  }

  encodeTwoSteps(filename, image, width, height);

  free(image);
}
