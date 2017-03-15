#ifndef WS_H
#define WS_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);

#endif /* WS_H */