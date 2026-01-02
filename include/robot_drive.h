#pragma once
#include <Arduino.h>
#include "motor.h"
#include "pins.h"

// Controls all 4 motors for robot drive system
class MotorController {
 public:
  MotorController()
      : motor1_(Pins::kMotorOutA1, Pins::kMotorOutB1, true, false),  // Left front
        motor2_(Pins::kMotorOutA2, Pins::kMotorOutB2, true, true),   // Right front, inverted
        motor3_(Pins::kMotorOutA3, Pins::kMotorOutB3, false, true),  // Left back, inverted
        motor4_(Pins::kMotorOutA4, Pins::kMotorOutB4, false, false) {} // Right back

  void begin() {
    applyToAllMotors([](Motor& motor) { motor.begin(); });
  }

  void forward(uint8_t speed = 255) {
    applyToAllMotors([speed](Motor& motor) { motor.forward(speed); });
  }

  void reverse(uint8_t speed = 255) {
    applyToAllMotors([speed](Motor& motor) { motor.reverse(speed); });
  }

  void turnLeft(uint8_t speed = 255) {
    left_motors_.reverse(speed);
    right_motors_.forward(speed);
  }

  void turnRight(uint8_t speed = 255) {
    left_motors_.forward(speed);
    right_motors_.reverse(speed);
  }

  void stop() {
    applyToAllMotors([](Motor& motor) { motor.stop(); });
  }

  // Individual motor access if needed
  Motor& motor1() { return motor1_; }
  Motor& motor2() { return motor2_; }
  Motor& motor3() { return motor3_; }
  Motor& motor4() { return motor4_; }

 private:
  struct MotorPair {
    Motor& front;
    Motor& back;
    
    void forward(uint8_t speed) {
      front.forward(speed);
      back.forward(speed);
    }
    
    void reverse(uint8_t speed) {
      front.reverse(speed);
      back.reverse(speed);
    }
  };

  template<typename Func>
  void applyToAllMotors(Func operation) {
    operation(motor1_);
    operation(motor2_);
    operation(motor3_);
    operation(motor4_);
  }

  Motor motor1_;
  Motor motor2_;
  Motor motor3_;
  Motor motor4_;
  
  MotorPair left_motors_{motor1_, motor3_};
  MotorPair right_motors_{motor2_, motor4_};
};
