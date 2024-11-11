#include "network.h"
#include <WiFi.h>
#include <HTTPClient.h>

bool sendRequest(const String& url, JsonDocument &doc)
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
    Serial.println(payload);
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

void connectToWiFi(const char* ssid, const char* password)
{
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(WiFi.status());
    Serial.print(" ");
  }

  Serial.println("\nConnected to WiFi");
  Serial.print("IP address: " + WiFi.localIP());
}