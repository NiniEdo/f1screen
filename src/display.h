#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <Adafruit_GFX.h>
#include <map>
#include <ArduinoJson.h>

extern bool status;

enum alignmentType
{
  LEFT,
  RIGHT,
  CENTER
};

enum class screenNameKeys
{
  HOME,
  STARTING,
  TEST,
  RACEWEEK,
};

void initDisplay();
void drawScreen(void (*drawPage)() = nullptr, void (*drawTopBar)() = nullptr);
void clearPage();
void drawString(int x, int y, String text, alignmentType alignment, bool wrap);
void drawStatus(int16_t x, int16_t y);
void drawBattery(int x, int y);
void drawTopBar();
void drawHomePage();
void drawRaceWeekPage();
void printSessionInfo(JsonObject &session, const std::map<String, String> &sessionsNameMap, const char *key, int xPosition, int &yPosition);
void drawSessionInfo(JsonArray &races, uint16_t index);
void setScreenName(screenNameKeys key);
void setScreenName(const String &name);
void drawTestPage();
void clearPage();

#endif // DISPLAY_H