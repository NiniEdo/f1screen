#include <Arduino.h>
#include <ArduinoJson.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <Adafruit_GFX.h>
#include "../fonts/Formula1_Bold5pt7b.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "wifi_credentials.h"
#include <map>

#define SCREEN_WIDTH 250
#define SCREEN_HEIGHT 122

// Display pins
static const uint8_t EPD_BUSY = 15;
static const uint8_t EPD_CS = 5;
static const uint8_t EPD_RST = 2;
static const uint8_t EPD_DC = 19;

const int TOP_BAR_HEIGHT = 12;

// Creating display object
GxEPD2_BW<GxEPD2_213_BN, GxEPD2_213_BN::HEIGHT> display(GxEPD2_213_BN(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));

bool status = true;
String screenName = "STARTING";
const std::map<String, String> API = {
    {"HOME_DATA", "http://"},
    {"test", "http://"}};

enum alignmentType
{
  LEFT,
  RIGHT,
  CENTER
};

void drawScreen(void (*drawPage)() = nullptr, void (*drawTopBar)() = nullptr)
{
  display.setFullWindow();
  display.firstPage();
  if (drawTopBar == nullptr && drawPage == nullptr)
  {
    return;
  }
  do
  {
    if (drawTopBar != nullptr)
    {
      display.drawRect(0, 0, SCREEN_WIDTH, TOP_BAR_HEIGHT, GxEPD_WHITE); // clears the top bar
      drawTopBar();
    }
    if (drawPage != nullptr)
    {
      display.drawRect(0, TOP_BAR_HEIGHT + 1, SCREEN_WIDTH, SCREEN_HEIGHT, GxEPD_WHITE); // clears the page
      drawPage();
    }
  } while (display.nextPage());
}
void drawString(int x, int y, String text, alignmentType alignment, bool wrap)
{
  int16_t x1, y1; // the bounds of x,y and w and h of the variable 'text' in pixels.
  uint16_t w, h;
  display.getTextBounds(text, x, y, &x1, &y1, &w, &h);
  display.setTextWrap(wrap);
  if (alignment == RIGHT)
    x = x - w;
  if (alignment == CENTER)
    x = x - w / 2;
  display.setCursor(x, y + h);
  display.print(text);
}
void drawStatus(int16_t x, int16_t y)
{
  display.setCursor(x, y);
  if (status)
  {
    drawString(x, y, "OK", RIGHT, false);
    display.fillCircle(SCREEN_WIDTH - 25, 12 / 2 - 2, 4, GxEPD_BLACK);
  }
  else
  {
    drawString(x, y, "KO", RIGHT, false);
    display.drawCircle(SCREEN_WIDTH - 25, 12 / 2 - 2, 4, GxEPD_BLACK);
  }
}
void drawBattery(int x, int y)
{
  uint8_t percentage = 100;
  float voltage = analogRead(35) / 4096.0 * 7.46;
  if (voltage > 1)
  { // Only display if there is a valid reading
    Serial.println("Voltage = " + String(voltage));
    percentage = 2836.9625 * pow(voltage, 4) - 43987.4889 * pow(voltage, 3) + 255233.8134 * pow(voltage, 2) - 656689.7123 * voltage + 632041.7303;
    if (voltage >= 4.20)
      percentage = 100;
    if (voltage <= 3.50)
      percentage = 0;
    // draw battery outline
    display.drawRect(x, y, 19, 10, GxEPD_BLACK);
    display.fillRect(x + 19 + 2, y + 2, 2, 6, GxEPD_BLACK);
    // fill battery
    display.fillRect(x + 2, y + 2, 15 * percentage / 100.0, 6, GxEPD_BLACK);
    drawString(x + 26, 1, String(percentage) + "%", LEFT, false);
  }
}
void drawTopBar()
{
  display.drawLine(0, TOP_BAR_HEIGHT, SCREEN_WIDTH, TOP_BAR_HEIGHT, GxEPD_BLACK);
  drawBattery(2, 0);
  drawStatus(SCREEN_WIDTH - 2, 0);
  drawString(SCREEN_WIDTH / 2, 0, screenName, CENTER, true);
}

bool sendRequest(String url, JsonDocument &doc)
{
  if (url == "")
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
    return false;
  }
  else
  {
    Serial.println("JSON parsing successfull!");
  }
  http.end();
  return true;
}

void drawHomePage()
{
  JsonDocument doc;
  bool isSuccessfull = sendRequest(API.at("HOME_DATA"), doc);
  status = isSuccessfull;

  if (isSuccessfull)
  {
    // draw data
  }
  
}
void drawTestPage()
{
  display.setCursor(20, 20);
  display.print("test");
}

void setup()
{
  Serial.begin(115200);
  display.init(115200, true, 50, false);
  display.setRotation(3);
  display.setFont(&Formula1_Bold5pt7b);
  display.setTextColor(GxEPD_BLACK);

  status = false;
  drawScreen(drawTopBar, nullptr);

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
  status = true;
}

void loop()
{
  // choose witch screen to show, currently for debug purposes
  int screen = 1;
  switch (screen)
  {
  case 1:
    screenName = "HOME";
    drawScreen(drawTestPage, drawTopBar);
    break;
  default:
    screenName = "TEST";
    drawScreen(drawTestPage, drawTopBar);
    break;
  }
}
