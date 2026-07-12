#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "weather_client.h"
#include "wifi_manager.h"
#include "pet_state.h"

static const char* WEATHER_URL =
    "https://restapi.amap.com/v3/weather/weatherInfo"
    "?key=ef86e2b18420d4017baf74d9b3d77c2b"
    "&city=110108&extensions=base&output=JSON";

static const unsigned long WEATHER_FETCH_INTERVAL_MS = 300000;
static const unsigned long HTTP_TIMEOUT_MS = 5000;

void weatherHandleStatusCode(int httpCode) {
    switch (httpCode) {
        case HTTP_CODE_OK:
            Serial.println("[Weather] HTTP 200 OK");
            break;
        case HTTP_CODE_NOT_FOUND:
            Serial.println("[Weather] HTTP 404 Not Found");
            break;
        case HTTP_CODE_INTERNAL_SERVER_ERROR:
            Serial.println("[Weather] HTTP 500 Server Error");
            break;
        case HTTP_CODE_SERVICE_UNAVAILABLE:
            Serial.println("[Weather] HTTP 503 Service Unavailable");
            break;
        default:
            Serial.print("[Weather] HTTP ");
            Serial.println(httpCode);
            break;
    }
}

static bool fetchWeather(PetState& s) {
    //创建对象，绑定url
    HTTPClient http;
    http.begin(WEATHER_URL);
    http.setTimeout(HTTP_TIMEOUT_MS);

    //发送get请求，处理状态码
    int httpCode = http.GET();
    weatherHandleStatusCode(httpCode);

    if (httpCode != HTTP_CODE_OK) {
        http.end();
        return false;
    }

    //获取响应体
    String payload = http.getString();
    http.end();

    //json解析
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, payload);
    if (err) {
        Serial.print("[Weather] JSON parse error: ");
        Serial.println(err.c_str());
        return false;
    }

    //验证api业务状态
    const char* status = doc["status"];
    if (!status || String(status) != "1") {
        Serial.println("[Weather] API status != 1");
        return false;
    }

    //提取天气和温度
    JsonObject lives0 = doc["lives"][0];
    const char* weather     = lives0["weather"]     | "--";
    const char* temperature = lives0["temperature"] | "--";

    //格式化写入天气和温度
    snprintf(s.cachedWeatherText, sizeof(s.cachedWeatherText),
             "%s %sC", weather, temperature);

    return true;
}

void weatherUpdate(PetState& s, unsigned long nowMs) {
    if (nowMs - s.lastFetchAttemptMs < WEATHER_FETCH_INTERVAL_MS) return;
    if (!wifiIsConnected()) return;

    s.lastFetchAttemptMs = nowMs;

    bool ok = fetchWeather(s);
    s.lastFetchOk = ok;

    if (ok) {
        Serial.print("[Weather] ");
        Serial.println(s.cachedWeatherText);
    }
}
