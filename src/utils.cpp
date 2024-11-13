#include <time.h>
#include <WiFi.h>
#include <stdio.h>
#include <ArduinoJson.h>

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

int findUpcomingDateIndex(const JsonArray &dates)
{
    struct tm currentTime = getTime();
    for (int i = 0; i < dates.size(); i++)
    {
        const char* dateStr = dates[i]["date"].as<const char*>();
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
