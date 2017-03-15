#include "api.h"

// This is where you define the API
void registerApiPaths(AsyncWebServer* server) {
  // Mask the networks file, and give scan information in its place
  server->on("/api/getVoltage", HTTP_GET, [](AsyncWebServerRequest* request) {
    String json = "{";

    json += "}";
    request->send(200, "text/json", json);
  });
}

void getSpectrum() {
  Serial.println("Getting a spectrum!!!!");
}