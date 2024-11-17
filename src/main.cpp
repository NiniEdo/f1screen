#include <Arduino.h>
#include "display.h"
#include "network.h"
#include "utils.h"
#include "wifi_credentials.h"
#include <TimeLib.h>

void setup()
{
  Serial.begin(115200);
  initDisplay();
  status = connectToWiFi(ssid, password);
  int screen = getScreenIndex();
  switch (screen)
  {
  case 0:
    setScreenName(screenNameKeys::RACEWEEK);
    drawScreen(drawRaceWeekPage, drawTopBar);
    delay(20000);
    break;
  case 1:
    setScreenName(screenNameKeys::HOME);
    drawScreen(drawHomePage, drawTopBar);
    delay(20000);
    break;
  default:
    setScreenName(screenNameKeys::TEST);
    drawScreen(drawTestPage, drawTopBar);
    break;
  }
}

void loop()
{
}