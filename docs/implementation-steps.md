# ESP32-S3 Smart Vitrine – Implementation Steps

This document describes the **recommended step-by-step order** to implement the ESP32-S3 Smart Vitrine firmware and embedded web app, using **PlatformIO in VSCode**.

The order is intentional: each step unlocks the next one while keeping the system testable and safe.

---

## 1. PlatformIO Project Base

- Create a new PlatformIO project for **ESP32-S3**
- Select the Arduino framework
- Verify upload and serial logging works
- Add basic logging utilities

---

## 2. Partition Table

- Create a custom `partitions.csv` including:
  - `ota_0`
  - `ota_1`
  - `littlefs`
- Configure `platformio.ini` to use this partition table

This step is mandatory before implementing OTA.

---

## 3. LittleFS Mount + Static Web Test

- Initialize and mount LittleFS at boot
- Add a simple `index.html` to LittleFS
- Serve it via HTTP
- Verify access from a phone browser using the device IP

---

## 4. SPA Fallback Routing

- Implement SPA routing rules:
  - Any non-API, non-static route returns `index.html`
- Ensure this works for paths like `/settings`, `/updates`, etc.

This is required for React Router.

---

## 5. Minimal REST API

- Implement:

  ```
  GET /api/info
  ```

- Return at least:
  - firmwareVersion
  - buildDate
  - uptime
  - IP address
  - LittleFS free space

This becomes the base communication contract with the web app.

---

## 6. WebSocket Base

- Implement WebSocket endpoint:

  ```
  /ws
  ```

- Allow connect / disconnect
- Add a basic message (ping/hello)
- Keep it non-blocking

---

## 7. Maintenance Mode

- Implement a global Maintenance Mode flag
- Optional: activate maintenance mode if a physical button is held at boot
- Rules when maintenance mode is ON:
  - WebSocket connections are rejected or closed
  - Hardware actions are paused or set to a safe state

This is required for safe OTA.

---

## 8. OTA Firmware Update

- Implement endpoint:

  ```
  POST /update/firmware
  ```

- Stream firmware binary using the Update API
- Do NOT buffer the full file in memory
- Flow:
  1. Enter maintenance mode
  2. Write firmware to inactive OTA partition
  3. Verify update
  4. Respond OK
  5. Reboot

Test multiple consecutive OTA updates to validate reliability.

---

## 9. React + Vite Web App (Production Build)

- Create the React app (Vite)
- Add screens:
  - Device Status
  - Updates
- Build with `vite build`
- Upload `dist/` to LittleFS
- Verify the ESP32 serves the production build correctly

---

## 10. OTA Web App (LittleFS)  (skipped)

- Implement endpoint:

  ```
  POST /update/web
  ```

- Recommended MVP approach:
  - Upload a **LittleFS image**
- Flow:
  1. Enter maintenance mode
  2. Stop WebSocket
  3. Block LittleFS access
  4. Write new filesystem
  5. Reboot

Avoid zip extraction in the first iteration.

---

## 11. LED Hardware Integration

- Implement `LedController`
- Expose controls via WebSocket / API:
  - On / Off
  - Brightness
  - Select shelf / miniature
- Verify real-time control from the web UI

---

## 12. NFC, Encoder, and Display

- Implement hardware modules:
  - NFC reader
  - Rotary encoder
  - Display
- Convert hardware events into state updates
- Broadcast relevant events via WebSocket

---

## 13. SD Card Integration (skipped)

- Mount SD card
- Read / write miniature metadata and positions
- Ensure SD access is non-blocking
- Avoid SD usage during OTA

---

## 14. Hardening & Cleanup

- Improve error handling
- Add timeouts and sanity checks
- Reduce logging noise
- Validate recovery after power loss during OTA

---

## Guiding Principle

At every step:

- Keep the device usable
- Prefer robustness over features
- Never break OTA once it works

This order ensures the vitrine can be maintained **100% wirelessly** from the very beginning.
