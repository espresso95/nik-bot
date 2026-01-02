#pragma once
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "pins.h"

// WiFi bridge to ESP32-CAM module via UART
// The ESP32-CAM should be programmed with AT command firmware or custom firmware
// that responds to simple commands over serial communication.
class WifiBridge {
 public:
  // Note: Hardware Serial (pins 0,1) is shared with USB programming.
  // Using SoftwareSerial on different pins for ESP32 communication is recommended.
  // However, if hardware serial is used, disconnect ESP32 during upload.
  WifiBridge(uint8_t rx_pin = 10, uint8_t tx_pin = 11, long baud_rate = 9600)
      : rx_pin_(rx_pin), tx_pin_(tx_pin), baud_rate_(baud_rate), 
        serial_(nullptr), connected_(false) {}

  ~WifiBridge() {
    if (serial_ != nullptr) {
      delete serial_;
    }
  }

  void begin() {
    // Initialize software serial for ESP32 communication
    serial_ = new SoftwareSerial(rx_pin_, tx_pin_);
    serial_->begin(baud_rate_);
    delay(100);  // Allow ESP32 to stabilize
    
    // Clear any pending data
    while (serial_->available()) {
      serial_->read();
    }
  }

  // Send a command to ESP32 and wait for response
  bool sendCommand(const char* command, uint16_t timeout_ms = 1000) {
    if (serial_ == nullptr) return false;
    
    // Clear buffer
    while (serial_->available()) {
      serial_->read();
    }
    
    // Send command
    serial_->println(command);
    
    // Wait for response
    unsigned long start = millis();
    while (millis() - start < timeout_ms) {
      if (serial_->available()) {
        return true;
      }
      delay(10);
    }
    return false;
  }

  // Connect to WiFi network
  bool connect(const char* ssid, const char* password, uint16_t timeout_ms = 10000) {
    if (serial_ == nullptr) return false;
    
    // Format: WIFI_CONNECT:ssid,password
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "WIFI_CONNECT:%s,%s", ssid, password);
    
    if (sendCommand(cmd, timeout_ms)) {
      connected_ = waitForResponse("OK", timeout_ms);
      return connected_;
    }
    return false;
  }

  // Disconnect from WiFi
  void disconnect() {
    if (serial_ == nullptr) return;
    sendCommand("WIFI_DISCONNECT");
    connected_ = false;
  }

  // Check if connected to WiFi
  bool isConnected() const {
    return connected_;
  }

  // Get IP address (returns via response parameter)
  bool getIpAddress(char* ip_buffer, size_t buffer_size, uint16_t timeout_ms = 2000) {
    if (serial_ == nullptr || !connected_) return false;
    
    sendCommand("GET_IP");
    return readResponse(ip_buffer, buffer_size, timeout_ms);
  }

  // Send HTTP GET request
  bool httpGet(const char* url, char* response_buffer, size_t buffer_size, uint16_t timeout_ms = 5000) {
    if (serial_ == nullptr || !connected_) return false;
    
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "HTTP_GET:%s", url);
    
    if (sendCommand(cmd, timeout_ms)) {
      return readResponse(response_buffer, buffer_size, timeout_ms);
    }
    return false;
  }

  // Send HTTP POST request
  bool httpPost(const char* url, const char* data, char* response_buffer, size_t buffer_size, uint16_t timeout_ms = 5000) {
    if (serial_ == nullptr || !connected_) return false;
    
    char cmd[384];
    snprintf(cmd, sizeof(cmd), "HTTP_POST:%s|%s", url, data);
    
    if (sendCommand(cmd, timeout_ms)) {
      return readResponse(response_buffer, buffer_size, timeout_ms);
    }
    return false;
  }

  // Read available data from serial
  int available() const {
    return (serial_ != nullptr) ? serial_->available() : 0;
  }

  // Read a single byte
  int read() {
    return (serial_ != nullptr) ? serial_->read() : -1;
  }

  // Write data to ESP32
  size_t write(const char* data) {
    return (serial_ != nullptr) ? serial_->print(data) : 0;
  }

  // Read a line from serial (up to newline or buffer size)
  bool readLine(char* buffer, size_t buffer_size, uint16_t timeout_ms = 1000) {
    if (serial_ == nullptr) return false;
    
    unsigned long start = millis();
    size_t index = 0;
    
    while (millis() - start < timeout_ms && index < buffer_size - 1) {
      if (serial_->available()) {
        char c = serial_->read();
        if (c == '\n') {
          buffer[index] = '\0';
          return true;
        }
        if (c != '\r') {  // Skip carriage return
          buffer[index++] = c;
        }
      }
      delay(1);
    }
    
    buffer[index] = '\0';
    return index > 0;
  }

 private:
  // Wait for specific response string
  bool waitForResponse(const char* expected, uint16_t timeout_ms) {
    char buffer[64];
    unsigned long start = millis();
    
    while (millis() - start < timeout_ms) {
      if (readLine(buffer, sizeof(buffer), timeout_ms - (millis() - start))) {
        if (strstr(buffer, expected) != nullptr) {
          return true;
        }
      }
    }
    return false;
  }

  // Read response into buffer
  bool readResponse(char* buffer, size_t buffer_size, uint16_t timeout_ms) {
    return readLine(buffer, buffer_size, timeout_ms);
  }

  uint8_t rx_pin_;
  uint8_t tx_pin_;
  long baud_rate_;
  SoftwareSerial* serial_;
  bool connected_;
};
