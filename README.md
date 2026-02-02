# ESP32-S3 Smart Vitrine (Miniatures Display Cabinet)

Firmware + embedded web UI for a smart miniatures display cabinet.

- ESP32-S3 drives a TFT (ST7789) + WS2812B LEDs + rotary encoder
- Optional NFC reader can read a tag and show miniature context
- Device serves a React/Vite web UI from the on-device filesystem
- WebSockets broadcast device events and allow remote control
- OTA firmware updates are supported via a streaming endpoint + maintenance mode

## Documentation

- Docs folder: [docs](docs)
- Project context: [docs/esp_32_s_3_smart_vitrine_project_context.md](docs/esp_32_s_3_smart_vitrine_project_context.md)
- Firmware architecture: [docs/esp_32_s_3_smart_vitrine_firmware_architecture_platform_io.md](docs/esp_32_s_3_smart_vitrine_firmware_architecture_platform_io.md)
- Implementation roadmap: [docs/implementation-steps.md](docs/implementation-steps.md)
- Modes & menu management: [docs/modes.md](docs/modes.md)

## What works today

- Local hardware UI
  - TFT displays the current miniature info
  - Rotary encoder changes the focused miniature
  - LEDs follow focus/selection
  - `BTN_MODE` opens a top-level modes menu and per-mode options
- NFC (if connected)
  - Read tag + parse/display common fields
- Networking
  - Wi-Fi station with AP fallback
  - HTTP server serving a SPA from the filesystem
  - WebSocket `/ws` for events and control
- Maintenance mode + OTA
  - Maintenance mode pauses hardware actions
  - OTA firmware update endpoint: `POST /update/firmware` (streaming)

## Hardware

Pin definitions live in [include/config.h](include/config.h).

- TFT (ST7789, SPI): CS/DC/RST + HW SPI SCK/MOSI + BLK
- WS2812B LED strip: one GPIO data line
- Rotary encoder: A/B + SW
- Extra button: `BTN_MODE` (opens the mode menu)

## Project structure (high level)

- Firmware entry point: [src/main.cpp](src/main.cpp)
- Hardware modules: [src/hardware](src/hardware)
- Networking/OTA/maintenance: [src/net](src/net)
- Web UI source (React/Vite): [web](web)
- Web UI build output uploaded to the device: [data](data)

## Build, flash, filesystem upload

This is a PlatformIO project.

If `pio` is not on PATH, you can run PlatformIO using the executable from the PlatformIO environment:

```powershell
cd "E:\PAULA\DOCUMENTOS\PlatformIO\Projects\ESP32-S3-MINIATURES-VITRINE"

# Build
C:\Users\pauco\.platformio\penv\Scripts\platformio.exe run

# Upload firmware (adjust COM port)
C:\Users\pauco\.platformio\penv\Scripts\platformio.exe run --target upload --upload-port COM9

# Upload filesystem image (serves the web UI from /data)
C:\Users\pauco\.platformio\penv\Scripts\platformio.exe run --target uploadfs --upload-port COM9
```

Partition table is in [partitions.csv](partitions.csv). The filesystem partition is named `spiffs` for PlatformIO `uploadfs` compatibility while the firmware mounts it via `LittleFS`.

## Web UI (React/Vite)

Source lives in [web](web). Production build artifacts are copied into [data](data) so they can be uploaded with `uploadfs`.

```powershell
cd "E:\PAULA\DOCUMENTOS\PlatformIO\Projects\ESP32-S3-MINIATURES-VITRINE\web"
npm install
npm run build

# Copies the build output into ../data
npm run copy-to-data
```

Then upload the filesystem with `uploadfs`.

## Notes

- MicroSD storage is planned (not implemented in the current code).
