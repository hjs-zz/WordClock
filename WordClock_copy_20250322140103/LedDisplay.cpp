#include "LedDisplay.h"

LedDisplay::LedDisplay()
    : pixels_(Config::LED_COUNT, Config::LED_PIN, NEO_GRB + NEO_KHZ800),
      animation_(Animation::IDLE),
      red_(255),
      green_(255),
      blue_(255),
      position_(0),
      phase_(0),
      candidateCount_(0),
      candidateIndex_(0),
      nextFrameAt_(0),
      hasTarget_(false) {}

void LedDisplay::begin() {
  pixels_.begin();
  pixels_.setBrightness(Config::LED_BRIGHTNESS);
  clearAndShow();
}

void LedDisplay::setColor(uint8_t colorState) {
  red_ = 0;
  green_ = 0;
  blue_ = 0;

  switch (colorState) {
    case 1:
      red_ = 255;
      break;
    case 2:
      green_ = 255;
      break;
    case 3:
      blue_ = 255;
      break;
    default:
      red_ = 255;
      green_ = 255;
      blue_ = 255;
      break;
  }
}

uint32_t LedDisplay::activeColor() const {
  return pixels_.Color(red_, green_, blue_);
}

uint8_t LedDisplay::ledAtPathPosition(uint8_t position) const {
  const uint8_t row = position / 11;
  const uint8_t column = position % 11;
  return row % 2 == 0 ? row * 11 + (10 - column) : row * 11 + column;
}

bool LedDisplay::frameIsDue(unsigned long now) const {
  return static_cast<long>(now - nextFrameAt_) >= 0;
}

void LedDisplay::clearAndShow() {
  for (uint8_t led = 0; led < Config::LED_COUNT; ++led) {
    pixels_.setPixelColor(led, 0);
  }
  pixels_.show();
}

void LedDisplay::copyTarget(const bool target[]) {
  for (uint8_t led = 0; led < Config::LED_COUNT; ++led) {
    target_[led] = target[led];
  }
  hasTarget_ = true;
}

void LedDisplay::renderTarget() {
  const uint32_t color = activeColor();
  for (uint8_t led = 0; led < Config::LED_COUNT; ++led) {
    pixels_.setPixelColor(led, target_[led] ? color : 0);
  }
  pixels_.show();
  animation_ = Animation::IDLE;
}

void LedDisplay::redrawTarget() {
  if (hasTarget_) {
    renderTarget();
  }
}

void LedDisplay::beginPathAnimation(Animation animation) {
  animation_ = animation;
  position_ = 0;
  phase_ = 0;
  nextFrameAt_ = millis();
  clearAndShow();
}

void LedDisplay::beginShuffle() {
  animation_ = Animation::SHUFFLE;
  candidateCount_ = 0;
  candidateIndex_ = 0;

  for (uint8_t led = 0; led < Config::LED_COUNT; ++led) {
    pixels_.setPixelColor(led, 0);
    if (target_[led]) {
      candidates_[candidateCount_++] = led;
    }
  }
  pixels_.show();

  for (int index = candidateCount_ - 1; index > 0; --index) {
    const int other = random(0, index + 1);
    const uint8_t temporary = candidates_[index];
    candidates_[index] = candidates_[other];
    candidates_[other] = temporary;
  }

  nextFrameAt_ = millis();
  if (candidateCount_ == 0) {
    animation_ = Animation::IDLE;
  }
}

void LedDisplay::startRandomTransition(const bool target[]) {
  copyTarget(target);

  switch (random(0, 6)) {
    case 0:
      beginPathAnimation(Animation::RUNNING_TEXT);
      break;
    case 1:
      beginPathAnimation(Animation::HORIZONTAL);
      break;
    case 2:
      renderTarget();
      break;
    case 3:
      beginPathAnimation(Animation::SNAKE);
      break;
    case 4:
      beginPathAnimation(Animation::WRITER);
      break;
    default:
      beginShuffle();
      break;
  }
}

void LedDisplay::updateRunningText(unsigned long now) {
  while (position_ < Config::LED_COUNT) {
    const uint8_t led = ledAtPathPosition(position_++);
    if (target_[led]) {
      pixels_.setPixelColor(led, activeColor());
      pixels_.show();
      nextFrameAt_ = now + 150;
      return;
    }
  }
  animation_ = Animation::IDLE;
}

void LedDisplay::updateHorizontal(unsigned long now) {
  const uint32_t color = activeColor();

  if (phase_ == 0) {
    for (uint8_t row = 0; row < 10; ++row) {
      const uint8_t led = row % 2 == 0 ? row * 11 + 10 - position_
                                       : row * 11 + position_;
      pixels_.setPixelColor(led, color);
    }
    pixels_.show();
    phase_ = 1;
    nextFrameAt_ = now + 60;
    return;
  }

  for (uint8_t row = 0; row < 10; ++row) {
    const uint8_t led = row % 2 == 0 ? row * 11 + 10 - position_
                                     : row * 11 + position_;
    pixels_.setPixelColor(led, target_[led] ? color : 0);
  }
  pixels_.show();

  ++position_;
  phase_ = 0;
  nextFrameAt_ = now;
  if (position_ >= 11) {
    animation_ = Animation::IDLE;
  }
}

void LedDisplay::updatePathAnimation(unsigned long now, bool useSnakePath) {
  const uint8_t led =
      useSnakePath ? ledAtPathPosition(position_) : position_;

  if (phase_ == 0) {
    pixels_.setPixelColor(led, activeColor());
    pixels_.show();
    phase_ = 1;
    nextFrameAt_ = now + 30;
    return;
  }

  pixels_.setPixelColor(led, target_[led] ? activeColor() : 0);
  pixels_.show();
  ++position_;
  phase_ = 0;
  nextFrameAt_ = now;
  if (position_ >= Config::LED_COUNT) {
    animation_ = Animation::IDLE;
  }
}

void LedDisplay::updateShuffle(unsigned long now, uint8_t frameDelay) {
  const uint8_t led = candidates_[candidateIndex_++];
  pixels_.setPixelColor(led, target_[led] ? activeColor() : 0);
  pixels_.show();
  nextFrameAt_ = now + frameDelay;

  if (candidateIndex_ >= candidateCount_) {
    animation_ = Animation::IDLE;
  }
}

void LedDisplay::update(unsigned long now) {
  if (animation_ == Animation::IDLE || !frameIsDue(now)) {
    return;
  }

  switch (animation_) {
    case Animation::RUNNING_TEXT:
      updateRunningText(now);
      break;
    case Animation::HORIZONTAL:
      updateHorizontal(now);
      break;
    case Animation::SNAKE:
      updatePathAnimation(now, true);
      break;
    case Animation::WRITER:
      updatePathAnimation(now, false);
      break;
    case Animation::SHUFFLE:
      updateShuffle(now, 100);
      break;
    case Animation::IDLE:
      break;
  }
}

void LedDisplay::showRtcError(bool visible) {
  animation_ = Animation::IDLE;
  for (uint8_t led = 0; led < Config::LED_COUNT; ++led) {
    pixels_.setPixelColor(led, 0);
  }

  if (visible) {
    const uint32_t errorColor = pixels_.Color(255, 0, 0);
    pixels_.setPixelColor(0, errorColor);
    pixels_.setPixelColor(10, errorColor);
    pixels_.setPixelColor(99, errorColor);
    pixels_.setPixelColor(109, errorColor);
  }
  pixels_.show();
}
