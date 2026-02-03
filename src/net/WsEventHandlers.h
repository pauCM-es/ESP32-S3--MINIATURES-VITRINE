#pragma once

#include "net/WsServer.h"
#include "hardware/LedControl.h"
#include "hardware/LedMovementControl.h"

class ModeManager;

// Attaches application-specific WS handlers (e.g., LED control) to the websocket server.
void attachWsEventHandlers(WsServer& wsServer, LedControl& ledControl, LedMovementControl& ledMovementControl, ModeManager* modeManager = nullptr);

// Broadcasts encoder/display events over WebSocket.
void broadcastEncoderRotate(WsServer& wsServer, int index);
void broadcastEncoderPress(WsServer& wsServer, int index);
void broadcastDisplayMiniature(WsServer& wsServer, int index);
