#pragma once

#include <ESPAsyncWebServer.h>

class WsServer {
public:
    using TextMessageHandler = void (*)(void* ctx, AsyncWebSocketClient* client, const char* message, size_t len);

    WsServer();
    
    void begin(AsyncWebServer *server);
    void setTextMessageHandler(void* ctx, TextMessageHandler handler);

    void handleEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, 
                     AwsEventType type, void *arg, uint8_t *data, size_t len);
    
    void broadcastMessage(const char *message);
    void closeAll();
    bool hasClients() const;

private:
    AsyncWebSocket ws;

    void* msgCtx = nullptr;
    TextMessageHandler msgHandler = nullptr;
    
    void handleConnect(AsyncWebSocketClient *client);
    void handleDisconnect(AsyncWebSocketClient *client);
    void handleTextMessage(AsyncWebSocketClient *client, uint8_t *data, size_t len);
};
