#pragma once

#include <Arduino.h>

namespace Config {
constexpr uint8_t BUTTON_PIN = 2;
constexpr uint8_t LED_PIN = 8;
constexpr uint8_t RANDOM_SEED_PIN = A0;
constexpr uint16_t LED_COUNT = 110;

// Full brightness is safe with the current transitions because no animation
// switches on the complete 110-pixel matrix at once.
constexpr uint8_t LED_BRIGHTNESS = 255;
constexpr uint8_t COLOR_COUNT = 4;
constexpr uint8_t COLOR_EEPROM_ADDRESS = 0;

constexpr unsigned long BUTTON_DEBOUNCE_MS = 250;
constexpr unsigned long CLOCK_REFRESH_MS = 1000;
constexpr unsigned long RTC_CHECK_MS = 10000;
constexpr unsigned long ERROR_BLINK_MS = 500;
}  // namespace Config
