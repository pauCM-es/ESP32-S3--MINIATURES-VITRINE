#include "OtaFirmware.h"

#include "MaintenanceMode.h"
#include "WsServer.h"
#include "../util/Log.h"

#include <ArduinoJson.h>
#include <Ticker.h>
#include <Update.h>

namespace {
struct OtaUploadContext {
    bool ok = true;
    bool begun = false;
    bool done = false;
    size_t written = 0;
    String error;
};

Ticker rebootTicker;
volatile bool otaBusy = false;

void scheduleRebootMs(uint32_t delayMs) {
    rebootTicker.once_ms(delayMs, []() {
        LOGW("ota", "Rebooting after OTA");
        ESP.restart();
    });
}
} // namespace

void OtaFirmware::attach(AsyncWebServer& server, WsServer& wsServer) {
    server.on(
        "/update/firmware",
        HTTP_POST,
        [](AsyncWebServerRequest* request) {
            auto* ctx = static_cast<OtaUploadContext*>(request->_tempObject);

            const bool ok = (ctx && ctx->done && ctx->ok && !Update.hasError());
            const String err = (!ctx) ? String("No OTA context")
                                      : (!ctx->error.isEmpty() ? ctx->error : String(Update.errorString()));

            JsonDocument doc;
            doc["ok"] = ok;
            if (ctx) {
                doc["written"] = static_cast<uint32_t>(ctx->written);
            }
            if (!ok) {
                doc["error"] = err;
            }

            String out;
            serializeJson(doc, out);

            request->send(ok ? 200 : 500, "application/json", out);

            if (ctx) {
                delete ctx;
                request->_tempObject = nullptr;
            }

            otaBusy = false;

            if (ok) {
                scheduleRebootMs(800);
            }
        },
        [&wsServer](AsyncWebServerRequest* request,
                   const String& filename,
                   size_t index,
                   uint8_t* data,
                   size_t len,
                   bool final) {
            auto* ctx = static_cast<OtaUploadContext*>(request->_tempObject);

            if (index == 0) {
                if (ctx) {
                    delete ctx;
                }
                ctx = new OtaUploadContext();
                request->_tempObject = ctx;

                if (otaBusy) {
                    ctx->ok = false;
                    ctx->done = true;
                    ctx->error = "OTA already in progress";
                    LOGW("ota", "Rejecting OTA upload (%s): busy", filename.c_str());
                    return;
                }

                otaBusy = true;

                LOGW("ota", "Starting OTA firmware upload: %s", filename.c_str());
                MaintenanceMode::getInstance().enter();
                wsServer.closeAll();

                if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH)) {
                    ctx->ok = false;
                    ctx->error = Update.errorString();
                    LOGE("ota", "Update.begin failed: %s", ctx->error.c_str());
                    return;
                }

                ctx->begun = true;
            }

            if (!ctx || !ctx->ok) {
                return;
            }

            if (len > 0) {
                const size_t writtenNow = Update.write(data, len);
                if (writtenNow != len) {
                    ctx->ok = false;
                    ctx->error = Update.errorString();
                    LOGE("ota", "Update.write failed: %s", ctx->error.c_str());
                    return;
                }
                ctx->written += writtenNow;
            }

            if (final) {
                if (ctx->begun) {
                    if (!Update.end(true)) {
                        ctx->ok = false;
                        ctx->error = Update.errorString();
                        LOGE("ota", "Update.end failed: %s", ctx->error.c_str());
                    } else {
                        LOGW("ota", "OTA finished. Written %u bytes", static_cast<unsigned>(ctx->written));
                    }
                } else {
                    ctx->ok = false;
                    ctx->error = "Update was not started";
                }

                if (!ctx->ok && ctx->begun) {
                    Update.abort();
                }

                ctx->done = true;
            }
        });
}
