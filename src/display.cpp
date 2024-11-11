#include "display.h"
#include "../fonts/Formula1_Bold5pt7b.h"
#include <ArduinoJson.h>
#include "network.h"

#define SCREEN_WIDTH 250
#define SCREEN_HEIGHT 122

static const uint8_t EPD_BUSY = 15;
static const uint8_t EPD_CS = 5;
static const uint8_t EPD_RST = 2;
static const uint8_t EPD_DC = 19;

const int TOP_BAR_HEIGHT = 12;

GxEPD2_BW<GxEPD2_213_BN, GxEPD2_213_BN::HEIGHT> display(GxEPD2_213_BN(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));

bool status = true;

const std::map<screenNameKeys, String> screenNameMap = {
    {screenNameKeys::HOME, "HOME"},
    {screenNameKeys::STARTING, "STARTING"},
};

const std::map<screenNameKeys, String> API = {
    {screenNameKeys::HOME, "http://"},
};

String screenName = screenNameMap.at(screenNameKeys::STARTING);

void initDisplay()
{
  display.init(115200, true, 50, false);
  display.setRotation(3);
  display.setFont(&Formula1_Bold5pt7b);
  display.setTextColor(GxEPD_BLACK);
  status = false;
  drawScreen(drawTopBar, nullptr);
}

void drawScreen(void (*drawPage)(), void (*drawTopBar)())
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

void drawHomePage()
{
  JsonDocument doc; // Inizializza con una dimensione appropriata
  bool isSuccessfull = sendRequest(API.at(screenNameKeys::HOME), doc);
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

void setScreenName(screenNameKeys key)
{
  screenName = screenNameMap.at(key);
}