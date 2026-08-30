#include "ClockLogic.h"

namespace {
struct LedRange {
  uint8_t first;
  uint8_t end;
};

constexpr LedRange WORD_FIVE = {0, 4};
constexpr LedRange WORD_IS = {5, 8};
constexpr LedRange WORD_IT = {9, 11};
constexpr LedRange WORD_TEN = {11, 15};
constexpr LedRange WORD_TWENTY = {15, 22};
constexpr LedRange WORD_QUARTER = {22, 29};
constexpr LedRange WORD_AFTER = {35, 39};
constexpr LedRange WORD_BEFORE = {39, 42};
constexpr LedRange WORD_HALF = {51, 55};
constexpr LedRange WORD_OCLOCK = {106, 109};

constexpr LedRange WORD_TWELVE = {45, 50};
constexpr LedRange WORD_TWO = {55, 59};
constexpr LedRange WORD_ONE = {57, 61};
constexpr LedRange WORD_ONE_OCLOCK = {57, 60};
constexpr LedRange WORD_SEVEN = {60, 66};
constexpr LedRange WORD_FIVE_HOUR = {66, 70};
constexpr LedRange WORD_THREE = {72, 76};
constexpr LedRange WORD_ELEVEN = {77, 80};
constexpr LedRange WORD_NINE = {80, 84};
constexpr LedRange WORD_FOUR = {84, 88};
constexpr LedRange WORD_TEN_HOUR = {90, 94};
constexpr LedRange WORD_EIGHT = {94, 98};
constexpr LedRange WORD_SIX = {100, 105};

constexpr LedRange HOUR_WORDS[12] = {
    WORD_TWELVE, WORD_ONE,      WORD_TWO,       WORD_THREE,
    WORD_FOUR,   WORD_FIVE_HOUR, WORD_SIX,       WORD_SEVEN,
    WORD_EIGHT,  WORD_NINE,     WORD_TEN_HOUR,  WORD_ELEVEN};

void setRange(bool target[], LedRange range) {
  for (uint8_t index = range.first; index < range.end; ++index) {
    target[index] = true;
  }
}

void clearPattern(bool target[]) {
  for (uint8_t index = 0; index < Config::LED_COUNT; ++index) {
    target[index] = false;
  }
}

uint8_t setMinuteWords(bool target[], uint8_t minuteValue) {
  setRange(target, WORD_IS);
  setRange(target, WORD_IT);

  const uint8_t minuteState = ((minuteValue + 2) / 5) % 12;
  switch (minuteState) {
    case 0:
      setRange(target, WORD_OCLOCK);
      break;
    case 1:
      setRange(target, WORD_FIVE);
      setRange(target, WORD_AFTER);
      break;
    case 2:
      setRange(target, WORD_TEN);
      setRange(target, WORD_AFTER);
      break;
    case 3:
      setRange(target, WORD_QUARTER);
      setRange(target, WORD_AFTER);
      break;
    case 4:
      setRange(target, WORD_TWENTY);
      setRange(target, WORD_AFTER);
      break;
    case 5:
      setRange(target, WORD_FIVE);
      setRange(target, WORD_BEFORE);
      setRange(target, WORD_HALF);
      break;
    case 6:
      setRange(target, WORD_HALF);
      break;
    case 7:
      setRange(target, WORD_FIVE);
      setRange(target, WORD_AFTER);
      setRange(target, WORD_HALF);
      break;
    case 8:
      setRange(target, WORD_TEN);
      setRange(target, WORD_AFTER);
      setRange(target, WORD_HALF);
      break;
    case 9:
      setRange(target, WORD_QUARTER);
      setRange(target, WORD_BEFORE);
      break;
    case 10:
      setRange(target, WORD_TEN);
      setRange(target, WORD_BEFORE);
      break;
    case 11:
      setRange(target, WORD_FIVE);
      setRange(target, WORD_BEFORE);
      break;
  }
  return minuteState;
}
}  // namespace

void buildDisplayState(DisplayState &state, uint8_t localHour,
                       uint8_t localMinute) {
  clearPattern(state.leds);
  state.minuteState = setMinuteWords(state.leds, localMinute);
  state.displayedHour =
      (localHour + (localMinute >= 23 ? 1 : 0)) % 12;

  const bool useOneWithoutS =
      state.displayedHour == 1 && state.minuteState == 0;
  setRange(state.leds,
           useOneWithoutS ? WORD_ONE_OCLOCK : HOUR_WORDS[state.displayedHour]);

  state.key = state.displayedHour * 12 + state.minuteState;
}
