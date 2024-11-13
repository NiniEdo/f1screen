#include "display.h"
#include "../fonts/Formula1_Bold5pt7b.h"
#include <ArduinoJson.h>
#include "network.h"
#include "utils.h"
#include <TimeLib.h>

#define SCREEN_WIDTH 250
#define SCREEN_HEIGHT 122

static const uint8_t EPD_BUSY = 15;
static const uint8_t EPD_CS = 5;
static const uint8_t EPD_RST = 2;
static const uint8_t EPD_DC = 19;

bool status = true;

const int TOP_BAR_HEIGHT = 12;

GxEPD2_BW<GxEPD2_213_BN, GxEPD2_213_BN::HEIGHT> display(GxEPD2_213_BN(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));

const std::map<screenNameKeys, String> screenNameMap = {
    {screenNameKeys::HOME, "HOME"},
    {screenNameKeys::STARTING, "STARTING"},
    {screenNameKeys::TEST, "TEST"}};

const std::map<String, String> API = {
    {"DriverStanding", "http://ergast.com/api/f1/current/driverStandings.json"},
    {"TeamsStanding", "http://ergast.com/api/f1/current/constructorStandings.json"},
    {"Calendar", "http://ergast.com/api/f1/current.json"},
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

void drawTable(int x, int y, int rows, int cols, int cellWidth[], int cellHeight)
{
  // horizontal lines
  for (int i = 0; i <= rows; i++)
  {
    int totalWidth = 0;
    for (int j = 0; j < cols; j++)
    {
      totalWidth += cellWidth[j];
    }
    display.drawLine(x, y + i * cellHeight, x + totalWidth, y + i * cellHeight, GxEPD_BLACK);
  }

  // vertical lines
  for (int j = 0; j <= cols; j++)
  {
    int currentX = x;
    for (int k = 0; k < j; k++)
    {
      currentX += cellWidth[k];
    }
    display.drawLine(currentX, y, currentX, y + rows * cellHeight, GxEPD_BLACK);
  }
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
  JsonDocument driverStandingDoc;
  bool driverRequestIsSuccessfull = sendRequest(API.at("DriverStanding"), driverStandingDoc);
  JsonDocument teamsStandingDoc;
  bool teamsRequestIsSuccessfull = sendRequest(API.at("TeamsStanding"), teamsStandingDoc);
  JsonDocument calendarDoc;
  bool calendarIsSuccessfull = sendRequest(API.at("Calendar"), calendarDoc);

  status = driverRequestIsSuccessfull && teamsRequestIsSuccessfull && calendarIsSuccessfull;

  if (status)
  {
    // print driver standings
    JsonArray driverStandings = driverStandingDoc["MRData"]["StandingsTable"]["StandingsLists"][0]["DriverStandings"].as<JsonArray>();
    int yPosition = 50;
    int CellWidth[] = {14, 40, 35};
    uint16_t startPoint = 16;
    int xPosition = startPoint;
    drawTable(xPosition - 2, yPosition - 5, 5, 3, CellWidth, 15);
    for (int i = 0; i < 5; i++)
    {
      uint16_t offset = 3;
      uint16_t xRelativePosition = xPosition;
      JsonObject driver = driverStandings[i];

      String name = driver["Driver"]["code"].as<String>();
      String points = driver["points"].as<String>();

      drawString(xRelativePosition, yPosition, String(i + 1), LEFT, false);
      xRelativePosition += CellWidth[0];

      drawString(xRelativePosition + offset, yPosition, name, LEFT, false);
      xRelativePosition += CellWidth[1];

      drawString(xRelativePosition + offset, yPosition, points, LEFT, false);
      yPosition += 15;
    }

    // print team standings
    JsonArray teamsStandings = teamsStandingDoc["MRData"]["StandingsTable"]["StandingsLists"][0]["ConstructorStandings"].as<JsonArray>();
    xPosition = CellWidth[0] + CellWidth[1] + CellWidth[2] + startPoint;
    yPosition = 50;
    CellWidth[0] = 95;
    CellWidth[1] = 35;
    drawTable(xPosition - 2, yPosition - 5, 5, 2, CellWidth, 15);
    for (int i = 0; i < 5; i++)
    {
      uint16_t offset = 3;
      uint16_t xRelativePosition = xPosition;
      JsonObject driver = teamsStandings[i];

      String name = driver["Constructor"]["name"].as<String>();
      name.replace("F1 Team", "");
      String points = driver["points"].as<String>();

      drawString(xRelativePosition + offset, yPosition, name, LEFT, false);
      xRelativePosition += CellWidth[0];

      drawString(xRelativePosition + offset, yPosition, points, LEFT, false);
      yPosition += 15;
    }

    // print future races
    JsonArray races = calendarDoc["MRData"]["RaceTable"]["Races"].as<JsonArray>();
    uint16_t index = findUpcomingDateIndex(races);
    String print = "";
    for (int i = index; i <= index + 1; i++)
    {
      String nextRaces = String(races[i]["date"].as<const char *>());
      String month = nextRaces.substring(5, 2);
      String day = nextRaces.substring(9, 2);
      print += day + "/" + month + " " + String(races[i]["Circuit"]["Location"]["country"].as<const char *>());
    }
    drawString(SCREEN_WIDTH / 2, 19, print, CENTER, false);
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