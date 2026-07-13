#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>

#define WIFI_SSID  "Tsinghua-Insecure"
#define WIFI_PASS  "yanyuanqing"

void wifiInit();
void wifiStart();
void wifiUpdate(unsigned long nowMs);
bool wifiIsConnected();

#endif
