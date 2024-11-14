#include <time.h>
#include <WiFi.h>
#include <stdio.h>
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
    const char *daysOfWeek[] = {"MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};
    return daysOfWeek[timeinfo.tm_wday];
}

tm LocalTime(tm raceTime, tm raceDate)
{
    bool isLegalTime = isDaylightSavingTime(raceDate.tm_year + 1900, raceDate.tm_mon + 1, raceDate.tm_mday);
    int baseHourTime = (raceTime.tm_hour + gmtOffset_sec / 3600) % 24;
    int newHourTime = isLegalTime ? baseHourTime + daylightOffset_sec / 3600 : baseHourTime;
    raceTime.tm_hour = newHourTime % 24;
    mktime(&raceTime);
    return raceTime;
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
            (date.tm_year == currentTime.tm_year && date.tm_mon == currentTime.tm_mon && date.tm_mday > currentTime.tm_mday))
        {
            return i;
        }
    }
    return 0;
}
bool isDaylightSavingTime(int year, int month, int day)
{
    int lastSundayMarch = 31 - ((5 + year * 5 / 4) % 7);
    int lastSundayOctober = 31 - ((2 + year * 5 / 4) % 7);

    if (month > 3 && month < 10)
    {
        return true;
    }
    else if (month == 3 && day >= lastSundayMarch)
    {
        return true;
    }
    else if (month == 10 && day < lastSundayOctober)
    {
        return true;
    }
    return false;
}