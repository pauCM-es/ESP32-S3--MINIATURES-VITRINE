#include "WsServer.h"
#include "../util/Log.h"
#include <ArduinoJson.h>

WsServer::WsServer() : ws("/ws") {}

void WsServer::begin(AsyncWebServer *server) {
    ws.onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, 
                      AwsEventType type, void *arg, uint8_t *data, size_t len) {
        this->handleEvent(server, client, type, arg, data, len);
    });
    
    server->addHandler(&ws);
    LOGI("ws", "WebSocket server initialized at /ws");
}

void WsServer::handleEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                           AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch (type) {
        case WS_EVT_CONNECT:
            handleConnect(client);
            break;
        case WS_EVT_DISCONNECT:
            handleDisconnect(client);
            break;
        case WS_EVT_DATA: {
            AwsFrameInfo *info = (AwsFrameInfo*)arg;
            if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
                handleTextMessage(client, data, len);
            }
            break;
        }
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}

void WsServer::handleConnect(AsyncWebSocketClient *client) {
    LOGI("ws", "Client #%u connected from %s", client->id(), client->remoteIP().toString().c_str());
    
    // Send welcome message
    JsonDocument doc;
    doc["type"] = "hello";
    doc["message"] = "Connected to ESP32-S3 Smart Vitrine";
    doc["clientId"] = client->id();
    
    String output;
    serializeJson(doc, output);
    client->text(output);
}

void WsServer::handleDisconnect(AsyncWebSocketClient *client) {
    LOGI("ws", "Client #%u disconnected", client->id());
}

void WsServer::handleTextMessage(AsyncWebSocketClient *client, uint8_t *data, size_t len) {
    data[len] = 0; // Null terminate
    LOGD("ws", "Received from #%u: %s", client->id(), (char*)data);
    
    // Parse incoming message
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, (char*)data);
    
    if (error) {
        LOGW("ws", "JSON parse error: %s", error.c_str());
        return;
    }
    
    const char* type = doc["type"];
    
    // Handle ping message
    if (type && strcmp(type, "ping") == 0) {
        JsonDocument response;
        response["type"] = "pong";
        response["timestamp"] = millis();
        
        String output;
        serializeJson(response, output);
        client->text(output);
    }
}

void WsServer::broadcastMessage(const char *message) {
    ws.textAll(message);
}

bool WsServer::hasClients() const {
    return ws.count() > 0;
}
