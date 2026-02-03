#include "net/WsEventHandlers.h"

#include <ArduinoJson.h>

#include "net/MaintenanceMode.h"
#include "util/Log.h"
#include "hardware/ModeManager.h"

struct WsLedContext {
    LedControl* ledControl;
    LedMovementControl* ledMovementControl;
    ModeManager* modeManager;
};

static WsLedContext g_ctx = {nullptr, nullptr, nullptr};

static void handleWsTextMessage(void* ctx, AsyncWebSocketClient* client, const char* message, size_t len) {
    (void)len;

    auto* c = static_cast<WsLedContext*>(ctx);
    if (!c || !c->ledControl || !c->ledMovementControl) {
        return;
    }

    if (MaintenanceMode::getInstance().isActive()) {
        JsonDocument response;
        response["type"] = "error";
        response["error"] = "maintenance";
        String out;
        serializeJson(response, out);
        client->text(out);
        return;
    }

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, message);
    if (err) {
        JsonDocument response;
        response["type"] = "error";
        response["error"] = "bad_json";
        String out;
        serializeJson(response, out);
        client->text(out);
        return;
    }

    const char* type = doc["type"];
    if (!type || strcmp(type, "led") != 0) {
        return;
    }

    const char* cmd = doc["cmd"];
    if (!cmd) {
        return;
    }

    // Commands:
    // {"type":"led","cmd":"brightness","value":0..100}
    // {"type":"led","cmd":"clear"}
    // {"type":"led","cmd":"pixel","index":0..N,"r":0..255,"g":0..255,"b":0..255}
    // {"type":"led","cmd":"mode","name":"standby"|"focus"|"selected","index":0..N}

    if (strcmp(cmd, "brightness") == 0) {
        int value = doc["value"] | -1;
        if (value < 0) value = 0;
        if (value > 100) value = 100;
        if (c->modeManager) {
            c->modeManager->setLedBrightnessPercent(static_cast<uint8_t>(value));
        } else {
            c->ledControl->setBrightness(static_cast<uint8_t>(value));
        }
    } else if (strcmp(cmd, "clear") == 0) {
        c->ledControl->clearAll();
    } else if (strcmp(cmd, "pixel") == 0) {
        int index = doc["index"] | -1;
        int r = doc["r"] | 0;
        int g = doc["g"] | 0;
        int b = doc["b"] | 0;
        if (index >= 0) {
            uint32_t color = c->ledControl->getColor(r & 0xFF, g & 0xFF, b & 0xFF, 0);
            c->ledControl->lightPosition(static_cast<uint8_t>(index), color);
        }
    } else if (strcmp(cmd, "mode") == 0) {
        const char* name = doc["name"];
        int index = doc["index"] | 0;
        if (name) {
            if (strcmp(name, "standby") == 0) {
                int brightness = doc["brightness"] | (doc["value"] | 50);
                if (brightness < 0) brightness = 0;
                if (brightness > 100) brightness = 100;
                if (c->modeManager) {
                    c->modeManager->setStandbyBrightnessPercent(static_cast<uint8_t>(brightness));
                }
                c->ledMovementControl->setStandbyMode(brightness);
            } else if (strcmp(name, "focus") == 0) {
                c->ledMovementControl->setFocusMode(static_cast<uint8_t>(index));
            } else if (strcmp(name, "selected") == 0) {
                c->ledMovementControl->setSelectedMode(static_cast<uint8_t>(index));
            }
        }
    } else {
        LOGW("ws", "Unknown led cmd: %s", cmd);
    }

    JsonDocument response;
    response["type"] = "ok";
    response["for"] = "led";
    response["cmd"] = cmd;
    String out;
    serializeJson(response, out);
    client->text(out);
}

void attachWsEventHandlers(WsServer& wsServer, LedControl& ledControl, LedMovementControl& ledMovementControl, ModeManager* modeManager) {
    g_ctx.ledControl = &ledControl;
    g_ctx.ledMovementControl = &ledMovementControl;
    g_ctx.modeManager = modeManager;

    wsServer.setTextMessageHandler(&g_ctx, handleWsTextMessage);
}

static void wsBroadcastJson(WsServer& wsServer, const JsonDocument& doc) {
    if (!wsServer.hasClients()) {
        return;
    }

    String out;
    serializeJson(doc, out);
    wsServer.broadcastMessage(out.c_str());
}

void broadcastEncoderRotate(WsServer& wsServer, int index) {
    JsonDocument doc;
    doc["type"] = "encoder";
    doc["event"] = "rotate";
    doc["index"] = index;
    doc["timestamp"] = millis();
    wsBroadcastJson(wsServer, doc);
}

void broadcastEncoderPress(WsServer& wsServer, int index) {
    JsonDocument doc;
    doc["type"] = "encoder";
    doc["event"] = "press";
    doc["index"] = index;
    doc["timestamp"] = millis();
    wsBroadcastJson(wsServer, doc);
}

void broadcastDisplayMiniature(WsServer& wsServer, int index) {
    JsonDocument doc;
    doc["type"] = "display";
    doc["event"] = "miniature";
    doc["index"] = index;
    doc["timestamp"] = millis();
    wsBroadcastJson(wsServer, doc);
}
