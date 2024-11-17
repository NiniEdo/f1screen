#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <map>

bool sendRequest(const String &url, JsonDocument &doc);
bool connectToWiFi(const char *ssid, const char *password);
extern const std::map<String, String> API;
#endif // NETWORK_H