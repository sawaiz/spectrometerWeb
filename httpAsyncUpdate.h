#ifndef HTTP_ASYNC_UPDATE_H
#define HTTP_ASYNC_UPDATE_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

class HttpAsyncUpdate{
    public:
        void setup(AsyncWebServer* server, char* updatePath, const char* username, const char* password);
        void handle();
    private:
};


#endif // HTTP_ASYNC_UPDATE_H