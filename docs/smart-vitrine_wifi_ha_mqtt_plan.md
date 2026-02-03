# Smart Vitrine — Connect to Home Wi‑Fi + Home Assistant (MQTT)  
*(ESP32‑S3 · PlatformIO · C++ · Web UI over HTTP + WebSocket)*

## Goal
- **Primary networking:** ESP32 joins your **normal home Wi‑Fi** (STA/client mode) and serves the existing React web app + WebSocket control channel.
- **Manual fallback provisioning:** enable/disable **SoftAP (“vitrine”)** from your device **Settings mode** (rotary encoder UI), instead of automatic fallback.
- **Home Assistant integration:** add **MQTT** with **HA MQTT Discovery** so HA can create entities automatically (lights/switches/sensors) and automate your vitrine.

---

## High-level architecture

### Network
- **STA mode (default):** connect to your router.
- **SoftAP (manual):** optional “vitrine” AP for provisioning / emergency access.
- **HTTP server:** serves the web app (SPIFFS/LittleFS) and API endpoints if you have any.
- **WebSocket server:** real-time UI ↔ device events (keep what you already have).

### Home Assistant
- **MQTT broker** (usually Mosquitto) runs on HA host.
- ESP32 publishes:
  - **state topics** (brightness, selected mini, shelf states, NFC UID, diagnostics…)
  - **availability** (LWT online/offline)
- ESP32 subscribes:
  - **command topics** to apply changes from HA
- ESP32 also publishes **Discovery config** payloads so HA auto-creates entities.

---

## Implementation plan (ordered steps)

### 0) Decide/confirm identifiers and topic schema
Pick stable IDs now—changing later is annoying.

**Device ID (stable):**
- `vitrine_id` (e.g. `vitrine_01`)  
- `device_name` for display (e.g. `Smart Vitrine`)

**Base topic:**
- `vitrine/<vitrine_id>/...`

**Availability topic (LWT):**
- `vitrine/<vitrine_id>/availability` (payload: `online` / `offline`)

---

### 1) Persist configuration in NVS (Wi‑Fi + MQTT + flags)
Store these in NVS:
- `wifi_ssid`, `wifi_pass`
- `mqtt_host`, `mqtt_port`
- `mqtt_user`, `mqtt_pass` (optional)
- `softap_enabled` (bool; your **Settings mode switch**)
- `mdns_enabled` (bool; optional)
- optional: `last_ip` (for debugging)

**Notes**
- On ESP32, NVS is the standard place for small config.
- If `wifi_ssid` is empty, device can still boot in “offline” mode and allow enabling SoftAP manually.

---

### 2) Wi‑Fi STA connection manager
Create a small module, e.g. `net/WifiManager.{h,cpp}`:

**Boot flow**
1. Start Wi‑Fi stack
2. If `wifi_ssid` configured:
   - Connect STA
   - Track connection state and IP
3. Expose events/callbacks:
   - `onConnected(ip)`
   - `onDisconnected(reason)`

**UI integration**
- In your **main modes list**, add a **Settings mode** item:
  - “Wi‑Fi: Connected / Disconnected”
  - “SoftAP: Off/On” (toggle)
  - “IP: 192.168.1.xxx”
  - “mDNS: vitrine.local” (if enabled)

---

### 3) Manual SoftAP enable/disable from Settings mode
Implement `enableSoftAP()` / `disableSoftAP()`.

**Recommended behavior**
- SoftAP is **OFF** by default.
- When user toggles ON:
  - Start SoftAP with SSID like `vitrine-<shortid>`
  - Use WPA2 password (not open)
  - Show SoftAP IP (usually `192.168.4.1`)
  - Optionally display a short help: “Connect to AP and open 192.168.4.1”
- When toggles OFF:
  - Stop SoftAP cleanly

**Important constraints**
- If you run **AP+STA simultaneously**, AP channel may follow the router channel and radio airtime is shared.
- If stability matters, consider:
  - Keep STA always on
  - Turn AP on only when needed
  - Disable AP after X minutes (optional safety timer)

---

### 4) Serve the web app in STA mode (unchanged, but verify binding)
Make sure the web server:
- Listens on all interfaces (STA + AP when enabled)
- Keeps working while STA reconnects (handle disconnect/reconnect)
- WebSocket should similarly bind on all interfaces

**Nice-to-have:** mDNS  
- Advertise `http://vitrine.local` on the LAN so you don’t need to find the IP.

---

### 5) Add MQTT client (core HA integration channel)
Add an MQTT client library suitable for ESP32/PlatformIO:
- Common choices: AsyncMqttClient, PubSubClient, esp-mqtt (if using ESP-IDF directly)

**Connection logic**
- Connect after STA is connected (or attempt anyway if broker is reachable via AP network).
- Set Last Will:
  - Topic: `.../availability`
  - Payload: `offline`
  - Retain: true
- On connect, immediately publish:
  - `.../availability` = `online` (retain true)
  - initial states (retain true for state topics)
  - discovery config (retain true)

**Reconnection**
- Implement exponential backoff.
- If Wi‑Fi drops, MQTT drops; reconnect when Wi‑Fi returns.

---

### 6) Define your HA entity model (keep it minimal)
To avoid entity explosion (7 shelves × 24 minis = 168), start small and expand.

**Suggested HA entities**
1. **Light (overall vitrine brightness)**  
   - Component: `light`  
   - Supports: on/off + brightness
2. **Switch: “Demo mode”**  
3. **Select: “Active shelf”** (1..7)  
4. **Sensor: “Selected mini id”**  
5. **Sensor: “Last NFC UID”**  
6. **Sensors (optional diagnostics):** RSSI, heap, uptime

**Keep per-mini control inside your web UI** unless you truly want HA automations per mini.

---

### 7) Implement MQTT topics (commands + state)
Example topics:

**State (device → HA)**
- `vitrine/<id>/state/brightness`  (0..255)
- `vitrine/<id>/state/demo`        (`0`/`1`)
- `vitrine/<id>/state/active_shelf` (1..7)
- `vitrine/<id>/state/selected_mini` (string)
- `vitrine/<id>/state/nfc_last_uid` (string)
- `vitrine/<id>/state/rssi` (int)
- `vitrine/<id>/state/free_heap` (int)

**Commands (HA → device)**
- `vitrine/<id>/cmd/brightness` (0..255)
- `vitrine/<id>/cmd/demo` (`0`/`1`)
- `vitrine/<id>/cmd/active_shelf` (1..7)
- `vitrine/<id>/cmd/highlight` (mini id / position payload)

**Rules**
- States: publish **retained** so HA restarts recover state quickly.
- Commands: usually **not retained**.

---

### 8) Publish Home Assistant MQTT Discovery configs
Home Assistant expects discovery messages under:
- `homeassistant/<component>/<unique_id>/config`

Each config JSON includes:
- `name`
- `unique_id`
- `state_topic`
- `command_topic` (for controllable entities)
- `availability_topic`
- `payload_available` / `payload_not_available`
- `device` block (ties entities to a single device in HA UI)

**Example discovery patterns**
- Light: `homeassistant/light/<id>_light/config`
- Switch: `homeassistant/switch/<id>_demo/config`
- Select: `homeassistant/select/<id>_active_shelf/config`
- Sensor: `homeassistant/sensor/<id>_nfc_uid/config`

**Retain discovery configs** so HA re-discovers on restart.

---

### 9) Bridge WebSocket events ↔ MQTT cleanly
You already have WebSocket events for the web UI. Keep them, but make a consistent “state source of truth” inside firmware:

**Recommended internal approach**
- Maintain a `DeviceState` struct:
  - brightness, demo, activeShelf, selectedMini, lastNfcUid…
- Any change (from UI, encoder, NFC, HA command) goes through:
  1. validate + apply to `DeviceState`
  2. update hardware (LEDs, display)
  3. publish updates to:
     - WebSocket clients (if connected)
     - MQTT state topic (retained)
- This prevents double logic and desync.

---

### 10) Security & reliability checklist
- **SoftAP password**: never leave it open.
- **MQTT credentials**: store in NVS; don’t hardcode.
- Consider **TLS MQTT** only if your broker supports it and you can handle certs (optional; start without).
- Add a **“Factory reset config”** action in Settings mode:
  - clears Wi‑Fi + MQTT config
  - disables SoftAP (or enables it, depending on your recovery preference)
- Watchdog / task health if you use async networking.
- Rate limit noisy sensors (RSSI every 10–30s, not every loop).

---

## Practical UI design for your Settings mode (encoder)
Suggested menu items:

1. **Wi‑Fi SSID**: `MyHomeWiFi` (read-only display)
2. **Wi‑Fi status**: Connected/Disconnected
3. **IP address**: `192.168.1.50`
4. **SoftAP**: Off/On  *(toggle)*  
5. **SoftAP SSID**: `vitrine-01` (read-only)
6. **SoftAP IP**: `192.168.4.1` (read-only)
7. **MQTT status**: Connected/Disconnected
8. **MQTT broker**: `192.168.1.10:1883` (read-only)
9. **mDNS**: Off/On (toggle)
10. **Factory reset** (action with confirmation)

---

## Home Assistant side setup (minimal)
1. Install/enable **Mosquitto broker** add-on (or any broker reachable on LAN).
2. Enable **MQTT integration** in HA.
3. Ensure discovery is enabled (default in HA MQTT integration).
4. Verify with HA logs:
   - device publishes discovery configs
   - entities appear automatically

---

## Testing plan (quick but effective)
1. **STA only**: connect to Wi‑Fi, open web app via IP.
2. **mDNS** (if enabled): open `vitrine.local`.
3. **MQTT connectivity**: see availability toggling online/offline.
4. **Discovery**: entities appear in HA.
5. **Command path**: toggle “Demo mode” in HA → device reacts.
6. **State path**: change brightness from encoder → HA updates.
7. **SoftAP manual**: toggle SoftAP ON, connect phone to AP, open `192.168.4.1`.

---

## Recommended file/module breakdown
- `net/WifiManager.*`
- `net/MdnsService.*` (optional)
- `mqtt/MqttClient.*`
- `ha/HaDiscovery.*` (build discovery payloads + publish)
- `state/DeviceState.*`
- `ui/SettingsMode.*` (encoder menu)

---

## Notes for future expansion
- Add **per-shelf lights** as additional `light` entities if you decide shelves should be independently controllable.
- Add **button entities** for “highlight selected mini”, “run showcase animation”, etc.
- If you ever want a deeper HA-native feel, you can later build a custom integration, but MQTT discovery often remains the simplest and most robust.
