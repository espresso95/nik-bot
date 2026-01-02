#pragma once
#include <Arduino.h>
#include "pins.h"

// Single DC motor controlled by an H-bridge (2 pins)
class Motor {
 public:
  Motor(uint8_t pin_a, uint8_t pin_b, bool pwm_capable = false, bool inverted = false)
      : pin_a_(pin_a), pin_b_(pin_b), pwm_capable_(pwm_capable), inverted_(inverted) {}

  void begin() {
    pinMode(pin_a_, OUTPUT);
    pinMode(pin_b_, OUTPUT);
    stop();
  }

  void forward(uint8_t speed = 255) {
    if (inverted_) {
      setMotorDirection(pin_b_, pin_a_, speed);
    } else {
      setMotorDirection(pin_a_, pin_b_, speed);
    }
  }

  void reverse(uint8_t speed = 255) {
    if (inverted_) {
      setMotorDirection(pin_a_, pin_b_, speed);
    } else {
      setMotorDirection(pin_b_, pin_a_, speed);
    }
  }

  void stop() {
    digitalWrite(pin_a_, LOW);
    digitalWrite(pin_b_, LOW);
  }

 private:
  void setMotorDirection(uint8_t active_pin, uint8_t inactive_pin, uint8_t speed) {
    digitalWrite(inactive_pin, LOW);
    
    if (pwm_capable_ && speed < 255) {
      analogWrite(active_pin, speed);
    } else {
      digitalWrite(active_pin, HIGH);
    }
  }

  uint8_t pin_a_;
  uint8_t pin_b_;
  bool pwm_capable_;
  bool inverted_;
};
