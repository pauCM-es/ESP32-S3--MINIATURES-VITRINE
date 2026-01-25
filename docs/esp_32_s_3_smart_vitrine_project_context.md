# Project Context – ESP32-S3 Smart Vitrine

## Purpose of this document
This document provides **technical context and architectural decisions** for an AI coding agent in VSCode. The goal is to help the agent generate **correct, consistent, and aligned code** for the ESP32-S3 firmware and its embedded web application.

The project targets a **smart miniature vitrine** controlled via a **web app served directly by the ESP32-S3**, with **OTA updates over Wi-Fi** for both firmware and web UI.

---

## Hardware
- Microcontroller: **ESP32-S3**
- Peripherals:
  - Individually addressable LEDs (SK6812, white channel only)
  - NFC reader
  - Display + rotary encoder + button
  - microSD card

---

## Development Environment
- IDE: **VSCode**
- Build system: **PlatformIO**
- Language: **C++**
- Framework: **Arduino framework for ESP32** (unless explicitly stated otherwise)

---

## High-Level Architecture

### ESP32-S3 Responsibilities
- Acts as a **standalone device** (no internet required)
- Hosts a **web server** that:
  - Serves a **React SPA (built with Vite)** from internal flash (LittleFS)
  - Exposes a **REST API** for configuration and state
  - Exposes a **WebSocket** endpoint for real-time control
- Manages all hardware logic:
  - LED control
  - NFC reading
  - Display and encoder interaction
- Supports **OTA updates over Wi-Fi**:
  - Firmware OTA (ESP32 partitions)
  - Web app OTA (LittleFS filesystem)

---

## Web Application (Frontend)

### Key Decisions
- The web app is a **SPA (Single Page Application)** built with **React + Vite**
- The **ESP32-S3 serves the built static files** (not the dev server)
- The app is accessed via browser at:
  - `http://device-ip/`
  - or `http://vitrina.local/` (mDNS, optional)
- The app may be installed as a **PWA** (Add to Home Screen)

### Routing Rules
- All non-API routes must return `index.html` (SPA fallback)
- Reserved paths:
  - `/api/*` → REST API
  - `/ws` → WebSocket
  - `/assets/*` → static files

---

## Filesystem

### LittleFS
- Used to store the **built web app** (`index.html`, JS, CSS, assets)
- Must be mounted at boot
- Web updates overwrite the filesystem contents

---

## OTA (Over-The-Air Updates)

### OTA Firmware Update
- Purpose: update ESP32 firmware (LED logic, NFC, APIs, etc.)
- Transport: **HTTP POST**
- Endpoint:
  - `POST /update/firmware`
- Payload:
  - `application/octet-stream`
  - Raw `.bin` firmware file
- Behavior:
  - Device switches to **maintenance mode**
  - Writes firmware to inactive OTA partition
  - Verifies write
  - Reboots

### OTA Web App Update
- Purpose: update the embedded web UI
- Transport: **HTTP POST**
- Endpoint:
  - `POST /update/web`
- Payload:
  - LittleFS image or packaged web assets
- Behavior:
  - Device switches to **maintenance mode**
  - Stops WebSocket and hardware interactions
  - Writes new files to LittleFS
  - Reboots or remounts filesystem

---

## Maintenance Mode

During OTA operations:
- Hardware control is temporarily disabled
- WebSocket connections are closed or rejected
- LEDs and animations should be paused or set to a safe state

Optionally:
- OTA endpoints may only be enabled if a **physical button is pressed at boot** (recovery / maintenance mode)

---

## API Design

### Required Endpoints

#### Device Info
```
GET /api/info
```
Returns JSON with:
- firmwareVersion
- webVersion
- buildDate
- uptime
- free LittleFS space

#### Control & Configuration
- Future endpoints for:
  - LED control
  - Brightness / intensity
  - Selected shelf / miniature
  - NFC-triggered actions

---

## WebSocket

### Endpoint
```
/ws
```

### Usage
- Real-time LED control
- Device state updates
- Encoder / button events

WebSocket should be disabled during maintenance mode.

---

## Versioning

- Firmware and web app versions are **independent**
- Versions should be hardcoded or injected at build time
- Versions must be exposed via `/api/info`

---

## Partitioning (Important)

The ESP32-S3 partition table must include:
- `ota_0`
- `ota_1`
- `littlefs`

This enables safe OTA updates without bricking the device.

---

## Development Workflow

### Development Mode
- React app runs locally (Vite dev server)
- App communicates with ESP32 via IP
- CORS or proxy configuration handled in Vite

### Production Mode
- `vite build` generates static files
- Files are uploaded to LittleFS
- ESP32 serves the production build

---

## Guiding Principles for the Agent

When generating code, always:
- Prefer **clarity and robustness** over cleverness
- Keep OTA logic **safe and isolated**
- Avoid blocking calls in the main loop
- Assume the device may lose power during OTA
- Design APIs and WebSocket messages to be **extensible**

This document defines the **source of truth** for architectural decisions in this project.

