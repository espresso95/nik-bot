# WiFi Component Usage Guide

## Overview

The WiFi component (`wifi.h`) provides a bridge to communicate with the ESP32-CAM module via UART for WiFi functionality. Since the Arduino Uno doesn't have built-in WiFi, this component enables network connectivity through the ESP32-CAM module included in the Zeus Car Shield kit.

## Hardware Setup

- **ESP32-CAM**: Connected to the Arduino Uno via UART
- **Communication**: Uses SoftwareSerial (recommended) or Hardware Serial
- **Default Pins**: RX=10, TX=11 (configurable)
- **Baud Rate**: 9600 (configurable)

**Important**: If using hardware serial (pins 0,1), disconnect the ESP32-CAM during Arduino sketch upload to avoid conflicts with USB programming.

## ESP32-CAM Firmware Requirements

The ESP32-CAM module must be programmed with firmware that responds to the commands sent by the WifiBridge class. The expected command format is:

- `WIFI_CONNECT:ssid,password` - Connect to WiFi network
- `WIFI_DISCONNECT` - Disconnect from WiFi
- `GET_IP` - Get IP address
- `HTTP_GET:url` - Send HTTP GET request
- `HTTP_POST:url|data` - Send HTTP POST request

The ESP32 should respond with:
- `OK` for successful operations
- IP address string for GET_IP
- HTTP response data for HTTP_GET/POST

## Basic Usage Example

```cpp
#include <Arduino.h>
#include "wifi.h"

// Create WiFi bridge instance
// Parameters: RX pin, TX pin, baud rate
WifiBridge wifi(10, 11, 9600);

void setup() {
  Serial.begin(9600);
  
  // Initialize WiFi bridge
  wifi.begin();
  delay(1000);
  
  // Connect to WiFi network
  Serial.println("Connecting to WiFi...");
  if (wifi.connect("YourSSID", "YourPassword", 15000)) {
    Serial.println("Connected to WiFi!");
    
    // Get IP address
    char ip[16];
    if (wifi.getIpAddress(ip, sizeof(ip))) {
      Serial.print("IP Address: ");
      Serial.println(ip);
    }
  } else {
    Serial.println("WiFi connection failed");
  }
}

void loop() {
  if (wifi.isConnected()) {
    // Send HTTP GET request
    char response[256];
    if (wifi.httpGet("http://api.example.com/data", response, sizeof(response))) {
      Serial.print("Response: ");
      Serial.println(response);
    }
    
    delay(5000);
  }
}
```

## Advanced Usage

### HTTP POST Request

```cpp
char response[256];
const char* url = "http://api.example.com/sensor";
const char* data = "{\"temperature\":25.5,\"humidity\":60}";

if (wifi.httpPost(url, data, response, sizeof(response))) {
  Serial.println(response);
}
```

### Direct Serial Communication

For custom commands not covered by the built-in methods:

```cpp
// Send custom command
wifi.write("CUSTOM_CMD:param1,param2\n");

// Read response
char buffer[128];
if (wifi.readLine(buffer, sizeof(buffer), 2000)) {
  Serial.println(buffer);
}
```

### Reading Raw Data

```cpp
while (wifi.available()) {
  char c = wifi.read();
  Serial.print(c);
}
```

## API Reference

### Constructor

```cpp
WifiBridge(uint8_t rx_pin = 10, uint8_t tx_pin = 11, long baud_rate = 9600)
```

Creates a WiFi bridge instance with specified pins and baud rate.

### Methods

#### `void begin()`
Initializes the serial communication with the ESP32-CAM module.

#### `bool connect(const char* ssid, const char* password, uint16_t timeout_ms = 10000)`
Connects to a WiFi network. Returns `true` on success.

#### `void disconnect()`
Disconnects from the current WiFi network.

#### `bool isConnected() const`
Returns `true` if connected to WiFi.

#### `bool getIpAddress(char* ip_buffer, size_t buffer_size, uint16_t timeout_ms = 2000)`
Retrieves the IP address into the provided buffer. Returns `true` on success.

#### `bool httpGet(const char* url, char* response_buffer, size_t buffer_size, uint16_t timeout_ms = 5000)`
Sends an HTTP GET request and stores the response in the buffer. Returns `true` on success.

#### `bool httpPost(const char* url, const char* data, char* response_buffer, size_t buffer_size, uint16_t timeout_ms = 5000)`
Sends an HTTP POST request with data and stores the response in the buffer. Returns `true` on success.

#### `bool sendCommand(const char* command, uint16_t timeout_ms = 1000)`
Sends a custom command to the ESP32. Returns `true` if response is received.

#### `int available() const`
Returns the number of bytes available to read from the serial buffer.

#### `int read()`
Reads a single byte from the serial buffer. Returns -1 if no data available.

#### `size_t write(const char* data)`
Writes data to the ESP32 via serial.

#### `bool readLine(char* buffer, size_t buffer_size, uint16_t timeout_ms = 1000)`
Reads a line of text (until newline) from the ESP32. Returns `true` on success.

## Integration with Robot

Example combining WiFi with other robot components:

```cpp
#include <Arduino.h>
#include "wifi.h"
#include "ultrasonic.h"
#include "rgb_led.h"

WifiBridge wifi(10, 11, 9600);
UltrasonicSensor ultrasonic;
RgbLed rgb;

void setup() {
  Serial.begin(9600);
  
  wifi.begin();
  ultrasonic.begin();
  rgb.begin();
  
  // Connect to WiFi
  if (wifi.connect("YourSSID", "YourPassword")) {
    rgb.setGreen();  // Green = WiFi connected
  } else {
    rgb.setRed();    // Red = WiFi failed
  }
}

void loop() {
  // Read ultrasonic distance
  long distance = ultrasonic.readDistanceCm();
  
  // Send sensor data via WiFi
  if (wifi.isConnected()) {
    char data[64];
    snprintf(data, sizeof(data), "{\"distance\":%ld}", distance);
    
    char response[128];
    wifi.httpPost("http://api.example.com/sensor", data, response, sizeof(response));
  }
  
  delay(1000);
}
```

## Troubleshooting

### WiFi Connection Fails
- Verify ESP32-CAM is powered and programmed with correct firmware
- Check baud rate matches between Arduino and ESP32
- Ensure correct SSID and password
- Increase timeout value

### No Response from ESP32
- Verify RX/TX pins are correctly connected
- Check if ESP32-CAM is running and not in deep sleep
- Try sending a simple command with longer timeout

### Upload Fails
- Disconnect ESP32-CAM from UART pins during Arduino upload if using hardware serial
- Use SoftwareSerial on different pins to avoid conflicts

## Notes

- The WifiBridge class uses dynamic memory allocation for SoftwareSerial. Ensure sufficient RAM is available.
- Long timeout values may block the main loop. Consider using non-blocking approaches for production code.
- The command protocol is customizable by programming the ESP32-CAM with different firmware.
