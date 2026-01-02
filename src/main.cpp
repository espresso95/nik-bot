#include <Arduino.h>

// Simple blink + serial heartbeat to confirm board and upload are working.

constexpr uint8_t kLedPin = LED_BUILTIN;  // Onboard LED on pin 13 for Uno.
constexpr unsigned long kBlinkIntervalMs = 500;

unsigned long lastToggle = 0;
bool ledState = false;

void setup() {
  pinMode(kLedPin, OUTPUT);

  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial to be ready (needed when using native USB boards; harmless on Uno).
  }

  Serial.println("nik-bot: booting...");
}

void loop() {
  const unsigned long now = millis();
  if (now - lastToggle >= kBlinkIntervalMs) {
    ledState = !ledState;
    digitalWrite(kLedPin, ledState ? HIGH : LOW);
    lastToggle = now;

    Serial.print("Heartbeat, LED is ");
    Serial.println(ledState ? "ON" : "OFF");
  }
}