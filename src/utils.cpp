#include <time.h>
#include <display.h>
#include <WiFi.h>
#include <stdio.h>
#include <network.h>
#include <ArduinoJson.h>
#include "utils.h"

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

tm getTime()
{
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm timeinfo;
    while (!getLocalTime(&timeinfo))
    {
        Serial.println("Attesa sincronizzazione orario...");
        delay(200);
    }
    return timeinfo;
}

tm StringToDate(const char *date)
{
    struct tm tm;
    strptime(date, "%Y-%m-%d", &tm);
    return tm;
}

tm StringToTime(const char *timeStr)
{
    struct tm tm;
    strptime(timeStr, "%H:%M:%S", &tm);
    return tm;
}

const char *getDayOfWeek(const tm &timeinfo)
{
    const char *daysOfWeek[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
    return daysOfWeek[timeinfo.tm_wday];
}

int findUpcomingDateIndex(const JsonArray &dates)
{
    struct tm currentTime = getTime();
    for (int i = 0; i < dates.size(); i++)
    {
        const char *dateStr = dates[i]["date"].as<const char *>();
        struct tm date = StringToDate(dateStr);
        if (date.tm_year > currentTime.tm_year ||
            (date.tm_year == currentTime.tm_year && date.tm_mon > currentTime.tm_mon) ||
            (date.tm_year == currentTime.tm_year && date.tm_mon == currentTime.tm_mon && date.tm_mday >= currentTime.tm_mday))
        {
            return i;
        }
    }
    return -1;
}

void LocalTime(tm &raceTime, tm &raceDate)
{
    bool isLegalTime = isDaylightSavingTime(raceDate.tm_year + 1900, raceDate.tm_mon + 1, raceDate.tm_mday);
    int totalOffsetHours = gmtOffset_sec / 3600 + (isLegalTime ? daylightOffset_sec / 3600 : 0);

    raceTime.tm_hour += totalOffsetHours;

    // this does not handle months and years, baybe it will in the future
    while (raceTime.tm_hour >= 24)
    {
        raceTime.tm_hour -= 24;
        raceTime.tm_mday += 1;
    }

    while (raceTime.tm_hour < 0)
    {
        raceTime.tm_hour += 24;
        raceTime.tm_mday -= 1;
    }
}

bool isDaylightSavingTime(int year, int month, int day)
{
    if (month < 3 || month > 10)
    {
        return false;
    }
    if (month > 3 && month < 10)
    {
        return true;
    }

    int lastSundayMarch = 31 - ((5 + year * 5 / 4) % 7);
    int lastSundayOctober = 31 - ((2 + year * 5 / 4) % 7);

    if (month == 3)
    {
        return day >= lastSundayMarch;
    }
    if (month == 10)
    {
        return day < lastSundayOctober;
    }
    return false;
}

int getScreenIndex()
{
    JsonDocument calendarDoc;
    bool calendarIsSuccessful = sendRequest(API.at("Calendar"), calendarDoc);

    status = calendarIsSuccessful;
    if (!status)
    {
        return -1;
    }

    JsonArray races = calendarDoc["MRData"]["RaceTable"]["Races"].as<JsonArray>();
    uint16_t index = findUpcomingDateIndex(races);

    if (index == -1)
    {
        return -1;
    }

    tm nextRaceDate = StringToDate(races[index]["date"].as<const char *>());
    tm currentTime = getTime();
    std::map<uint8_t, uint8_t> daysOfWeekMap = {
        {0, 6}, {1, 0}, {2, 1}, {3, 2}, {4, 3}, {5, 4}, {6, 5}};
    uint16_t yearDayOfRace = nextRaceDate.tm_yday;
    uint8_t weekDayOfRace = nextRaceDate.tm_wday;
    uint16_t yearDayOfMondayOfRaceWeek = yearDayOfRace - daysOfWeekMap[weekDayOfRace];    
    if (currentTime.tm_yday < yearDayOfMondayOfRaceWeek)
    {
        // return home screen
        return 1;
    }
    else if (currentTime.tm_yday >= yearDayOfMondayOfRaceWeek && currentTime.tm_yday <= yearDayOfRace)
    {
        // return raceweek screen
        return 0;
    }
    else
    {
        // return home screen
        return 1;
    }
}
void enterDeepSleep(int sleepSeconds)
{
    Serial.println("Entering deep sleep...");
    hibernateDisplay();
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
    esp_sleep_enable_timer_wakeup(sleepSeconds * 1000000);
    esp_deep_sleep_start();
}
int secondsUntilMidnight(){
    tm currentTime = getTime();
    uint64_t secondsUntilMidnight = 86400 - (currentTime.tm_hour * 3600 + currentTime.tm_min * 60 + currentTime.tm_sec);
    Serial.print("Seconds until midnight: ");
    Serial.print(secondsUntilMidnight);
    return secondsUntilMidnight;
}