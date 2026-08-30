#pragma once

#include <Arduino.h>

#include "Config.h"

struct DisplayState {
  bool leds[Config::LED_COUNT];
  uint8_t minuteState;
  uint8_t displayedHour;
  uint8_t key;
};

void buildDisplayState(DisplayState &state, uint8_t localHour,
                       uint8_t localMinute);
