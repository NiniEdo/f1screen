#include <Arduino.h>
#include "display.h"
#include "network.h"
#include "wifi_credentials.h"

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
  int screen = 1;
  switch (screen)
  {
  case 1:
    setScreenName(screenNameKeys::HOME);
    drawScreen(drawHomePage, drawTopBar);
    delay(20000);
    break;
  default:
    setScreenName("TEST");
    drawScreen(drawTestPage, drawTopBar);
    break;
  }
}