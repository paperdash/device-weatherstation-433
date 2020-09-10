#include <WiFiClient.h>
#include <SPIFFS.h>
#include "display.h"

// File webFile;
char servername[] = "192.168.178.65";
//char servername[] = "192.168.178.56"; // charles proxy
WiFiClient client;


#define MTU_Size 2 * 1460 // this size seems to work best

byte postFile(const char *fileName, const char *servername, uint16_t port);

void setupDisplay()
{
	postFile("/tmp2.jpeg", "192.168.178.65", 80);
}

void loopDisplay()
{
}

byte postFile(const char *fileName, const char *servername, uint16_t port)
{
	// connect
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
	}

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