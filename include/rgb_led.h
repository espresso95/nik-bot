#pragma once
#include <Arduino.h>
#include "pins.h"

class RgbLed {
 public:
  RgbLed() = default;

  void begin() {
    pinMode(Pins::kRgbRed, OUTPUT);
    pinMode(Pins::kRgbGreen, OUTPUT);
    pinMode(Pins::kRgbBlue, OUTPUT);
    off();
  }

  void setColor(bool red, bool green, bool blue) {
    setPin(Pins::kRgbRed, red);
    setPin(Pins::kRgbGreen, green);
    setPin(Pins::kRgbBlue, blue);
  }

  void on() { setColor(true, true, true); }
  void off() { setColor(false, false, false); }

  void setRed() { setColor(true, false, false); }
  void setGreen() { setColor(false, true, false); }
  void setBlue() { setColor(false, false, true); }

  void toggle() {
    is_on_ = !is_on_;
    is_on_ ? on() : off();
  }

 private:
  void setPin(uint8_t pin, bool state) {
    digitalWrite(pin, state ? HIGH : LOW);
  }
  
  bool is_on_ = false;
};
