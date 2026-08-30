#include "RtcClock.h"

#include <string.h>

namespace {
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};
TimeChangeRule CET = {"CET", Last, Sun, Oct, 3, 60};
}  // namespace

RtcClock::RtcClock() : rtc_(), timezone_(CEST, CET) {}

bool RtcClock::isValid() {
  return rtc_.oscillatorCheck();
}

time_t RtcClock::localNow() {
  const DateTime utcNow = RTClib::now();
  return timezone_.toLocal(utcNow.unixtime());
}

void RtcClock::setFromLocalCompileTime(const char *compileDate,
                                       const char *compileTime) {
  int year;
  int month;
  int day;
  int hourValue;
  int minuteValue;
  int secondValue;
  char monthName[4];
  const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

  sscanf(compileDate, "%3s %d %d", monthName, &day, &year);
  sscanf(compileTime, "%d:%d:%d", &hourValue, &minuteValue, &secondValue);

  for (month = 1; month <= 12; ++month) {
    if (strncmp(monthName, months[month - 1], 3) == 0) {
      break;
    }
  }

  const DateTime localCompileTime(year, month, day, hourValue, minuteValue,
                                  secondValue);
  const time_t utcCompileTime =
      timezone_.toUTC(localCompileTime.unixtime());
  rtc_.setEpoch(utcCompileTime);
  Serial.println(F("RTC initialized with UTC compile time."));
}
