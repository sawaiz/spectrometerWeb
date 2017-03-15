#ifndef CONNECT_H
#define CONNECT_H

#include <Arduino.h>
#include <String.h>
#include <Stream.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>

// Try to connect to known networks
int multiConnect(int numTries);
int addNetwork(String ssid, String password);
int checkDuplicate(String newSsid);
int removeNetwork(String ssid);
String knownNetworksJson();

// Regitser netwrok paths
void registerNetworkConfigPaths(AsyncWebServer* server);

// Utilities
int safeOpen(char* filename, char* type);

#endif // CONNECT_H