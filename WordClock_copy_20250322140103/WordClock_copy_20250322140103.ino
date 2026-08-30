#include <EEPROM.h>
#include <Wire.h>

#include "ClockLogic.h"
#include "Config.h"
#include "LedDisplay.h"
#include "RtcClock.h"

LedDisplay display;
RtcClock clockService;
DisplayState currentDisplay;

volatile bool buttonPressed = false;
uint8_t colorState = 0;
int lastDisplayKey = -1;
bool rtcValid = false;
bool errorVisible = false;
unsigned long lastButtonPress = 0;
unsigned long lastClockRefresh = 0;
unsigned long lastRtcCheck = 0;
unsigned long lastErrorToggle = 0;

void onButtonPressed() {
  buttonPressed = true;
}

void handleButton(unsigned long now) {
  noInterrupts();
  const bool wasPressed = buttonPressed;
  buttonPressed = false;
  interrupts();

  if (!wasPressed) {
    return;
  }

  if (lastButtonPress != 0 &&
      now - lastButtonPress < Config::BUTTON_DEBOUNCE_MS) {
    return;
  }

  lastButtonPress = now;
  colorState = (colorState + 1) % Config::COLOR_COUNT;
  EEPROM.update(Config::COLOR_EEPROM_ADDRESS, colorState);
  display.setColor(colorState);

  if (rtcValid) {
    display.redrawTarget();
  }
}

void checkRtc(unsigned long now) {
  if (now - lastRtcCheck < Config::RTC_CHECK_MS) {
    return;
  }
  lastRtcCheck = now;

  const bool wasValid = rtcValid;
  rtcValid = clockService.isValid();

  if (rtcValid == wasValid) {
    return;
  }

  if (rtcValid) {
    Serial.println(F("RTC is running again."));
    lastDisplayKey = -1;
  } else {
    Serial.println(F("RTC error: oscillator stopped or time is invalid."));
    display.showRtcError(true);
    errorVisible = true;
  }
}

void updateRtcError(unsigned long now) {
  if (now - lastErrorToggle < Config::ERROR_BLINK_MS) {
    return;
  }

  lastErrorToggle = now;
  errorVisible = !errorVisible;
  display.showRtcError(errorVisible);
}

void updateClock(unsigned long now) {
  if (now - lastClockRefresh < Config::CLOCK_REFRESH_MS) {
    return;
  }
  lastClockRefresh = now;

  const time_t localNow = clockService.localNow();
  buildDisplayState(currentDisplay, hour(localNow), minute(localNow));

  if (currentDisplay.key != lastDisplayKey) {
    display.startRandomTransition(currentDisplay.leds);
    lastDisplayKey = currentDisplay.key;
  }
}

void setup() {
  Serial.begin(57600);
  Wire.begin();

  display.begin();
  colorState = EEPROM.read(Config::COLOR_EEPROM_ADDRESS);
  if (colorState >= Config::COLOR_COUNT) {
    colorState = 0;
  }
  display.setColor(colorState);

  randomSeed(analogRead(Config::RANDOM_SEED_PIN) ^ micros());

  pinMode(Config::BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Config::BUTTON_PIN), onButtonPressed,
                  FALLING);

  // Enable once to initialize a new or invalid RTC, upload, then comment again:
  // clockService.setFromLocalCompileTime(__DATE__, __TIME__);

  rtcValid = clockService.isValid();
  const unsigned long now = millis();
  lastRtcCheck = now;
  lastClockRefresh = now - Config::CLOCK_REFRESH_MS;

  if (rtcValid) {
    Serial.println(F("Word clock ready."));
  } else {
    Serial.println(F("RTC error: initialize the clock before normal use."));
    display.showRtcError(true);
    errorVisible = true;
  }
}

void loop() {
  const unsigned long now = millis();

  handleButton(now);
  display.update(now);
  checkRtc(now);

  if (rtcValid) {
    updateClock(now);
  } else {
    updateRtcError(now);
  }
}
