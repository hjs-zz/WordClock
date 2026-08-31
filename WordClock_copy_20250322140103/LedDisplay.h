#pragma once

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#include "Config.h"

class LedDisplay {
 public:
  LedDisplay();

  void begin();
  void setColor(uint8_t colorState);
  void startRandomTransition(const bool target[]);
  void update(unsigned long now);
  void redrawTarget();
  void showRtcError(bool visible);

 private:
  enum class Animation : uint8_t {
    IDLE,
    RUNNING_TEXT,
    HORIZONTAL,
    SNAKE,
    WRITER,
    SHUFFLE
  };

  Adafruit_NeoPixel pixels_;
  Animation animation_;
  bool target_[Config::LED_COUNT];
  uint8_t candidates_[Config::LED_COUNT];
  uint8_t red_;
  uint8_t green_;
  uint8_t blue_;
  uint8_t position_;
  uint8_t phase_;
  uint8_t candidateCount_;
  uint8_t candidateIndex_;
  unsigned long nextFrameAt_;
  bool hasTarget_;

  uint32_t activeColor() const;
  uint8_t ledAtPathPosition(uint8_t position) const;
  bool frameIsDue(unsigned long now) const;
  void clearAndShow();
  void renderTarget();
  void copyTarget(const bool target[]);
  void beginPathAnimation(Animation animation);
  void beginShuffle();
  void updateRunningText(unsigned long now);
  void updateHorizontal(unsigned long now);
  void updatePathAnimation(unsigned long now, bool useSnakePath);
  void updateShuffle(unsigned long now, uint8_t frameDelay);
};
