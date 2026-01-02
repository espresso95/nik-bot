#include <Arduino.h>
#include "rgb_led.h"
#include "robot_drive.h"

RgbLed rgb;
MotorController motors;

void setup() {
  rgb.begin();
  motors.begin();
}

void loop() {
  rgb.setGreen();
  motors.forward(100);
  delay(500);
  
  motors.stop();
  rgb.off();
  delay(500);
  
  rgb.setRed();
  motors.reverse(100);
  delay(500);
  
  motors.stop();
  rgb.off();
  delay(500);
}