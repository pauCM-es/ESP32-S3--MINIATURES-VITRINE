#pragma once

#include <ESPAsyncWebServer.h>

class WsServer {
public:
    WsServer();
    
    void begin(AsyncWebServer *server);
    void handleEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, 
                     AwsEventType type, void *arg, uint8_t *data, size_t len);
    
    void broadcastMessage(const char *message);
    bool hasClients() const;

private:
    AsyncWebSocket ws;
    
    void handleConnect(AsyncWebSocketClient *client);
    void handleDisconnect(AsyncWebSocketClient *client);
    void handleTextMessage(AsyncWebSocketClient *client, uint8_t *data, size_t len);
};
