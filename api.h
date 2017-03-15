#ifndef API_H
#define API_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

void getSpectrum();

void registerApiPaths(AsyncWebServer * server);

#endif /* API_H */