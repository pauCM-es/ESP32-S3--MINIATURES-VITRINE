# Firmware Architecture – ESP32-S3 Smart Vitrine (PlatformIO)

## Purpose

This document describes the recommended **firmware architecture**, folder structure, modules, and implementation rules for the ESP32-S3 Smart Vitrine.

Goals:

- Clean separation between **hardware logic**, **networking**, and **UI hosting**
- Safe and reliable **OTA updates** (firmware + web filesystem)
- Maintainable codebase for long-term feature growth

---

## Tooling / Constraints

- MCU: ESP32-S3
- PlatformIO in VSCode
- C++
- Arduino framework for ESP32
- LittleFS used to store and serve the built React web app
- OTA performed over Wi-Fi via HTTP endpoints

---

## Recommended PlatformIO Project Layout

```
firmware/
  platformio.ini
  partitions.csv
  include/
    version.h
  src/
    main.cpp
    app/
      App.h
      App.cpp
      State.h
    net/
      WifiManager.h
      WifiManager.cpp
      MdnsService.h
      MdnsService.cpp
      WebServer.h
      WebServer.cpp
      ApiRoutes.h
      ApiRoutes.cpp
      WsServer.h
      WsServer.cpp
      MaintenanceMode.h
      MaintenanceMode.cpp
      OtaFirmware.h
      OtaFirmware.cpp
      OtaWebFs.h
      OtaWebFs.cpp
    storage/
      LittleFsManager.h
      LittleFsManager.cpp
      SdManager.h
      SdManager.cpp
    hardware/
      LedController.h
      LedController.cpp
      NfcManager.h
      NfcManager.cpp
      DisplayManager.h
      DisplayManager.cpp
      EncoderManager.h
      EncoderManager.cpp
    util/
      Log.h
      Log.cpp
      TimeUtil.h
      TimeUtil.cpp
      JsonUtil.h
      JsonUtil.cpp
  data/
    (LittleFS content for dev/testing)
```

Notes:

- `src/app` holds the top-level orchestration and device state.
- `src/net` contains everything related to HTTP, REST, WebSocket, OTA, maintenance.
- `src/hardware` contains all hardware drivers/controllers.
- `src/storage` handles LittleFS and SD card.
- `include/version.h` provides version strings compiled into the firmware.

---

## Core Modules and Responsibilities

### 1) App Orchestrator (src/app)

**App** is responsible for:

- Boot sequence and initialization order
- Owning global state (selected shelf, selected miniature, brightness, etc.)
- Running periodic tasks in loop without blocking
- Delegating:
  - hardware polling (encoder, NFC)
  - network servicing

Recommended patterns:

- A lightweight `State` struct (plain data)
- Controllers update State; networking reads/writes State via well-defined methods

### 2) Hardware Layer (src/hw)

Hardware modules should:

- Be testable in isolation
- Avoid direct dependencies on networking
- Expose simple APIs:
  - `LedController::setBrightness(uint8_t)`
  - `LedController::selectMiniature(int shelf, int slot)`
  - `NfcManager::poll()` returning events
  - `EncoderManager::poll()` returning events

Avoid:

- Long blocking delays
- Serial print spam; use a logging helper

### 3) Storage Layer (src/storage)

#### LittleFsManager

Responsibilities:

- Mount LittleFS
- Provide stats (total, used, free)
- Optionally list files for debugging

#### SdManager

Responsibilities:

- Mount SD card
- Read/write miniature metadata and positions
- Provide safe file access methods

Important rule:

- During OTA WebFS updates, **disable any concurrent LittleFS reads/writes**.

### 4) Networking Layer (src/net)

#### WifiManager

- Connect to Wi-Fi (station mode)
- Provide IP, RSSI, SSID
- Optional: fallback AP mode (future)

#### MdnsService (optional but recommended)

- Advertise `vitrina.local`
- Publish HTTP service

#### WebServer

- Serve static files from LittleFS
- Implement SPA fallback: any non-API route -> `index.html`
- Expose:
  - `/api/*` REST routes
  - `/ws` WebSocket
  - `/update/*` OTA endpoints

Implementation options:

- `ESPAsyncWebServer` (recommended for non-blocking IO)
- Or Arduino `WebServer` (simpler but can block under load)

#### ApiRoutes

- Register REST endpoints and handlers
- Mandatory:
  - `GET /api/info`

#### WsServer

- Handle real-time messages
- Broadcast device state changes
- Reject connections during maintenance mode

---

## Maintenance Mode (src/net/MaintenanceMode)

Maintenance mode is an explicit device state used to perform OTA safely.

Rules when maintenance mode is ON:

- WebSocket must be closed or rejected
- Hardware actions should be paused or put into a safe state
- SD and LittleFS writes should be blocked (except OTA target)

Optional policy:

- OTA endpoints are only enabled when:
  - a physical button is held at boot, OR
  - a short-lived auth token is granted

---

## OTA Implementation

### Partitions

A custom `partitions.csv` must include:

- `ota_0`
- `ota_1`
- `littlefs`

### OTA Firmware (src/net/OtaFirmware)

Endpoint:

- `POST /update/firmware`

Behavior:

- Enter maintenance mode
- Stream incoming bytes to Update API
- Verify result
- Respond OK
- Reboot

Key requirements:

- Stream in chunks (do not buffer whole binary)
- Validate content length if available
- Provide progress events (optional) via SSE or WS (future)

### OTA Web Filesystem (src/net/OtaWebFs)

Endpoint:

- `POST /update/web`

Two acceptable approaches:

1) Upload a **LittleFS image** built by PlatformIO and write it (simple + robust)
2) Upload a **zip** and extract (more complex; not recommended initially)

Recommended initial approach:

- Use LittleFS image
- Replace filesystem content atomically (as much as possible)

Key requirements:

- Enter maintenance mode
- Close WebSocket
- Ensure no file handles are open
- Write FS image / overwrite content
- Reboot

---

## Versioning

`include/version.h` should define:

- `FIRMWARE_VERSION` (e.g., "0.1.0")
- `WEB_VERSION` (optional; can be embedded into web build and returned by `/api/info`)
- `BUILD_DATE` / `GIT_SHA` (optional)

`GET /api/info` must return:

- firmwareVersion
- webVersion
- buildDate
- uptime
- littlefsFree
- ip

---

## Boot Sequence (Recommended Order)

1. Init serial logging
2. Read hardware pins (maintenance button)
3. Mount LittleFS
4. Mount SD (optional; can be deferred)
5. Init hardware controllers (LEDs, display, encoder, NFC)
6. Connect Wi-Fi
7. Start mDNS (optional)
8. Start web server + routes + WS
9. Enter main loop (poll hardware + maintain network)

---

## Non-Blocking Loop Guidelines

- No `delay()` except tiny debounces if unavoidable
- Use millis-based scheduling
- Poll encoder/NFC at fixed intervals
- Keep network server responsive

---

## Logging

- Centralize logging in `util/Log.*`
- Support levels: INFO/WARN/ERROR/DEBUG
- Avoid excessive logs in tight loops

---

## Security (Minimum Viable)

Initial recommended MVP:

- OTA endpoints require maintenance mode enabled at boot (physical button)

Future:

- Add token-based auth for `/update/*`
- Add pairing flow in UI

---

## Agent Guidance

When generating code:

- Keep modules independent (hw does not depend on net)
- Add clear headers and comments
- Prefer `ESPAsyncWebServer` for stability
- Implement OTA endpoints with streamed writes
- Always respect maintenance mode constraints
