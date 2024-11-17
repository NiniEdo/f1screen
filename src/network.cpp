#include "network.h"
#include <WiFi.h>
#include <WiFi.h>
#include <HTTPClient.h>


const std::map<String, String> API = {
    {"DriverStanding", "http://ergast.com/api/f1/current/driverStandings.json"},
    {"TeamsStanding", "http://ergast.com/api/f1/current/constructorStandings.json"},
    {"Calendar", "http://ergast.com/api/f1/current.json"},
};

bool sendRequest(const String &url, JsonDocument &doc)
{
  if (url.isEmpty())
    return false;

  HTTPClient http;
  String payload;
  http.begin(url);
  int httpCode = http.GET();
  if (httpCode > 0 && httpCode == HTTP_CODE_OK)
  {
    payload = http.getString();
  }
  else
  {
    Serial.println("Error on HTTP request, status code: " + String(httpCode));
    http.end();
    return false;
  }

  DeserializationError error = deserializeJson(doc, payload);
  if (error || doc.isNull())
  {
    Serial.println("JSON parsing failed!");
    http.end();
    return false;
  }
  else
  {
    Serial.println("JSON parsing successful!");
  }
  http.end();
  return true;
}

bool connectToWiFi(const char *ssid, const char *password)
{
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000)
  {
    delay(500);
    Serial.print(WiFi.status());
    Serial.print(" ");
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    IPAddress dns(8, 8, 8, 8);
    WiFi.config(WiFi.localIP(), WiFi.gatewayIP(), WiFi.subnetMask(), dns);

    Serial.println("\nConnected to WiFi");
    Serial.println(WiFi.localIP());
    return true;
  }
  else
  {
    Serial.println("\nFailed to connect to WiFi");
    return false;
  }
}