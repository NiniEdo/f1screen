#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include <ArduinoJson.h>

bool sendRequest(const String& url, JsonDocument &doc);
void connectToWiFi(const char* ssid, const char* password);

#endif // NETWORK_H