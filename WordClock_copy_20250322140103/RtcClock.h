#pragma once

#include <Arduino.h>
#include <DS3231.h>
#include <Timezone.h>

class RtcClock {
 public:
  RtcClock();

  bool isValid();
  time_t localNow();
  void setFromLocalCompileTime(const char *compileDate,
                               const char *compileTime);

 private:
  DS3231 rtc_;
  Timezone timezone_;
};
