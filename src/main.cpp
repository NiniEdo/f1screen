#include <Arduino.h>
#include "display.h"
#include "network.h"
#include "wifi_credentials.h"
#include <TimeLib.h>

void setup()
{
  Serial.begin(115200);
  initDisplay();
  connectToWiFi(ssid, password);
  status = true;
}

void loop()
{
  // choose which screen to show, currently for debug purposes
  int screen = 2;
  switch (screen)
  {
  case 1:
    setScreenName(screenNameKeys::HOME);
    drawScreen(drawHomePage, drawTopBar);
    delay(20000);
    break;
  case 2:
    setScreenName(screenNameKeys::RACEWEEK);
    drawScreen(drawRaceWeekPage, drawTopBar);
    delay(20000);
    break;
  default:
    setScreenName(screenNameKeys::TEST);
    drawScreen(drawTestPage, drawTopBar);
    break;
  }
}