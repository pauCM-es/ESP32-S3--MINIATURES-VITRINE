#pragma once

#include "net/WsServer.h"
#include "hardware/LedControl.h"
#include "hardware/LedMovementControl.h"

// Attaches application-specific WS handlers (e.g., LED control) to the websocket server.
void attachWsEventHandlers(WsServer& wsServer, LedControl& ledControl, LedMovementControl& ledMovementControl);
