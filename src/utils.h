tm getTime();
tm StringToDate(const char *date);
int findUpcomingDateIndex(const JsonArray &dates);
const char* getDayOfWeek(const tm &timeinfo);
tm StringToTime(const char *timeStr);
void LocalTime(tm &raceTime, tm &raceDate);
bool isDaylightSavingTime(int year, int month, int day);

