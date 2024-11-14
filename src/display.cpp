#include <ArduinoJson.h>
#include <TimeLib.h>
#include <cctype>
#include <GxEPD2_BW.h>
#include "display.h"
#include "../fonts/Formula1_Bold5pt7b.h"
#include "network.h"
#include "utils.h"

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
    {screenNameKeys::TEST, "TEST"},
    {screenNameKeys::RACEWEEK, "RACE WEEK"}};

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
    if (drawPage != nullptr)
    {
      display.drawRect(0, TOP_BAR_HEIGHT + 1, SCREEN_WIDTH, SCREEN_HEIGHT - TOP_BAR_HEIGHT, GxEPD_WHITE); // clears the page
      drawPage();
    }
    if (drawTopBar != nullptr)
    {
      display.drawRect(0, 0, SCREEN_WIDTH, TOP_BAR_HEIGHT, GxEPD_WHITE); // clears the top bar
      drawTopBar();
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
  bool driverRequestIsSuccessful = sendRequest(API.at("DriverStanding"), driverStandingDoc);
  JsonDocument teamsStandingDoc;
  bool teamsRequestIsSuccessful = sendRequest(API.at("TeamsStanding"), teamsStandingDoc);
  JsonDocument calendarDoc;
  bool calendarIsSuccessful = sendRequest(API.at("Calendar"), calendarDoc);

  status = driverRequestIsSuccessful && teamsRequestIsSuccessful && calendarIsSuccessful;

  if (!status)
  {
    return;
  }
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
  String raceDetails[2];
  uint8_t ystart = 16;
  for (int i = 0; i < 2; i++)
  {
    uint8_t raceNumber = index + i;
    if (raceNumber < races.size())
    {
      String nextRaces = String(races[raceNumber]["date"].as<const char *>());
      struct tm date = StringToDate(nextRaces.c_str());
      String month = String(date.tm_mon);
      String day = String(date.tm_mday);
      raceDetails[i] += day + "/" + month + " " +
                        String(races[raceNumber]["Circuit"]["Location"]["locality"].as<const char *>()) + " (" +
                        String(races[raceNumber]["Circuit"]["Location"]["country"].as<const char *>()) + ")";

      drawString(SCREEN_WIDTH / 2, ystart, raceDetails[i], CENTER, false);
      ystart += 13;
    }
  }
}


void drawRaceWeekPage()
{
  JsonDocument calendarDoc;
  bool calendarIsSuccessful = sendRequest(API.at("Calendar"), calendarDoc);

  status = calendarIsSuccessful;
  if (!status)
  {
    return;
  }

  JsonArray races = calendarDoc["MRData"]["RaceTable"]["Races"].as<JsonArray>();
  uint16_t index = findUpcomingDateIndex(races);

  drawSessionInfo(races, index);

  String raceName = races[index]["raceName"].as<String>();
  raceName.toUpperCase();
  drawString(SCREEN_WIDTH / 2, 20, raceName, CENTER, false);
}

void drawSessionInfo(JsonArray &races, uint16_t index)
{
  if (index < races.size())
  {
    const std::map<String, String> sessionsNameMap = {
        {"FirstPractice", "FP1"},
        {"ThirdPractice", "FP3"},
        {"SecondPractice", "FP2"},
        {"Qualifying", "QUALI"},
        {"Race", "RACE"},
        {"Sprint", "PRINT"}};

    JsonObject race = races[index];

    int yPosition = 60;
    int xPosition = 5;

    for (JsonPair kv : race)
    {
      const char *key = kv.key().c_str();

      if (strcmp(key, "season") == 0 ||
          strcmp(key, "round") == 0 ||
          strcmp(key, "url") == 0 ||
          strcmp(key, "raceName") == 0 ||
          strcmp(key, "Circuit") == 0 ||
          strcmp(key, "date") == 0 ||
          strcmp(key, "time") == 0)
      {
        continue;
      }
      if (race[key].is<JsonObject>())
      {
        JsonObject session = race[key].as<JsonObject>();
        printSessionInfo(session, sessionsNameMap, key, xPosition, yPosition);
      }
    }
    JsonObject session = races[index].as<JsonObject>();
    printSessionInfo(session, sessionsNameMap, "RACE", xPosition, yPosition);
  }
}

void printSessionInfo(JsonObject &session, const std::map<String, String> &sessionsNameMap, const char *key, int xPosition, int &yPosition)
{
  const char *dateStr = session["date"];
  const char *timeStr = session["time"];

  if (dateStr == nullptr || timeStr == nullptr)
  {
    return;
  }

  tm date = StringToDate(dateStr);
  tm time = StringToTime(timeStr);
  String day = getDayOfWeek(date);
  tm localtime = LocalTime(time, date);

  String sessionName = sessionsNameMap.count(String(key)) ? sessionsNameMap.at(String(key)) : String(key);
  String sessionInfo = day + " " + String(localtime.tm_hour) + " " + (localtime.tm_min < 10 ? "0" : "") + String(localtime.tm_min);

  drawString(xPosition, yPosition, sessionName, LEFT, false);
  drawString(xPosition + 50, yPosition, sessionInfo, LEFT, false);

  yPosition += 13;
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