#include <WiFi.h>
#include "wifi_manager.h"
#include "pet_state.h"

static unsigned long lastReconnectMs = 0;
static const unsigned long RECONNECT_INTERVAL_MS = 10000;
static bool s_started = false;

void wifiInit() {
    Serial.println("[WiFi] Initializing...");
    WiFi.mode(WIFI_STA);
}

void wifiStart() {
    Serial.println("[WiFi] Starting connection...");
    s_started = true;
    WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void wifiUpdate(unsigned long nowMs) {
    bool connected = (WiFi.status() == WL_CONNECTED);

    if (connected != state.wifiConnected) {
        state.wifiConnected = connected;
        if (connected) {
            Serial.print("[WiFi] Connected, IP: ");
            Serial.println(WiFi.localIP());
        } else {
            Serial.println("[WiFi] Disconnected");
        }
    }

    if (!connected && s_started && (nowMs - lastReconnectMs >= RECONNECT_INTERVAL_MS)) {
        lastReconnectMs = nowMs;
        Serial.println("[WiFi] Attempting reconnect...");
        WiFi.reconnect();
    }
}

bool wifiIsConnected() {
    return WiFi.status() == WL_CONNECTED;
}
