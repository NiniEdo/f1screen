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
  screen = 1;
  switch (screen)
  {
  case 0:
    setScreenName(screenNameKeys::RACEWEEK);
    drawScreen(drawRaceWeekPage, drawTopBar);
    break;
  case 1:
    setScreenName(screenNameKeys::HOME);
    drawScreen(drawHomePage, drawTopBar);
    break;
  default:
    setScreenName(screenNameKeys::ERROR);
    drawScreen(drawErrorPage, drawTopBar);
    break;
  }  
  enterDeepSleep(secondsUntilMidnight());
}

void loop()
{
}