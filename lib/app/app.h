#ifndef APP_H
#define APP_H

#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>

void setupApp();
void loopApp();

void sendDataWs(DynamicJsonDocument doc);

#endif