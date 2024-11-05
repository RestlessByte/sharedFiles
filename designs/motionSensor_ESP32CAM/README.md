# Digital Security System

This project implements a digital security system using an ESP32, a PIR motion sensor, and integration with Telegram for notifications.

## Table of Contents
- [Overview](#overview)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Setup Instructions](#setup-instructions)
- [Code](#code)
- [Usage](#usage)
- [License](#license)

## Overview
The Digital Security System detects motion using a PIR sensor and sends an alert via Telegram. It also sends data to a specified web server.

## Hardware Requirements
- ESP32 Development Board
- PIR Motion Sensor
- LED (For checks work)
- Jumper wires
- Breadboard

## Software Requirements
- Arduino IDE
- ESP32 board support for Arduino IDE
- Libraries: `WiFi`, `WiFiClientSecure`, `UniversalTelegramBot`, `ArduinoJson`, `HTTPClient`

## Setup Instructions

### 1. Hardware Setup
- Connect the PIR sensor to the ESP32:
  - VCC to 3.3V
  - GND to GND
  - OUT to GPIO 23
- Connect the LED to the ESP32:
  - Anode (+) to GPIO 4 (with a current limiting resistor)
  - Cathode (-) to GND

### 2. Software Setup
- Open the Arduino IDE.
- Install the required libraries via the Library Manager:
  - `WiFi`
  - `WiFiClientSecure`
  - `UniversalTelegramBot`
  - `ArduinoJson`
  - `HTTPClient`
- Add the ESP32 board support to the Arduino IDE.

### 3. Configuration
Create a `config.h` file in the project directory with the following content:

```cpp
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define BOT_TOKEN "GetToken in bot @BotFather"
#define CHAT_ID "YourChatID"
#define SERVER_URL "YourWebServerURL"
