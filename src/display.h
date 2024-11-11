#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <Adafruit_GFX.h>
#include <map>

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
};

void initDisplay();
void drawScreen(void (*drawPage)() = nullptr, void (*drawTopBar)() = nullptr);
void drawString(int x, int y, String text, alignmentType alignment, bool wrap);
void drawStatus(int16_t x, int16_t y);
void drawBattery(int x, int y);
void drawTopBar();
void drawHomePage();
void drawTestPage();
void setScreenName(screenNameKeys key);
void setScreenName(const String& name);

#endif // DISPLAY_H