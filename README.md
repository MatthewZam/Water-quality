# Water Quality Monitoring IoT System

A real-time water quality monitoring system using ESP32 that measures multiple water parameters and determines water potability with cloud data logging.

## 🌊 Overview

This IoT-based water quality monitoring system continuously monitors water parameters including turbidity, TDS (Total Dissolved Solids), temperature, and pH levels. The system provides real-time feedback on water quality and sends data to Firebase for remote monitoring and analysis.

## ✨ Features

- **Real-time Monitoring**: Continuous measurement of water quality parameters
- **Multi-parameter Analysis**: 
  - Turbidity (water clarity)
  - TDS (Total Dissolved Solids)
  - Temperature
  - pH levels
- **Local Display**: Real-time readings on 16x2 LCD display
- **Cloud Integration**: Data logging to Firebase Realtime Database
- **WiFi Connectivity**: Wireless data transmission via ESP32

## 🛠️ Hardware Components

| Component | Model/Type | Quantity |
|-----------|------------|----------|
| Microcontroller | ESP32 DOIT DevKit | 1 |
| Turbidity Sensor | Analog Turbidity Sensor | 1 |
| TDS Sensor | Analog TDS Meter | 1 |
| pH Sensor | Analog pH Sensor | 1 |
| Display | LCD 16x2 with I2C Module | 1 |
| Power Supply | 5V/3.3V Power Adapter | 1 |
| Jumper Wires | Male-to-Male/Female | As needed |
| Breadboard | Half/Full Size | 1 |

## 📋 Water Quality Thresholds

The system uses the following thresholds to determine water potability:

| Parameter | Good Range | Unit |
|-----------|------------|------|
| Turbidity | < 1 NTU | NTU |
| TDS | 50-300 ppm | ppm |
| Temperature | 15-25°C | °C |
| pH | 6.5-8.5 | pH |

## 🚀 Installation & Setup

### Prerequisites
- Arduino IDE (v1.8.x or v2.x)
- ESP32 Board Package
- Required Libraries (see below)

### Required Libraries
Install these libraries through Arduino IDE Library Manager:

```
- WiFi (ESP32 built-in)
- FirebaseESP32 by Mobizt
- LiquidCrystal_I2C
- OneWire (for temperature sensor)
- DallasTemperature (for DS18B20)
```

### Configuration Steps

1. **Clone the Repository**
   ```bash
   git clone https://github.com/yourusername/water-quality-monitoring-iot.git
   cd water-quality-monitoring-iot
   ```

2. **Configure WiFi & Firebase**
   - Copy `config_template.h` to `config.h`
   - Update your WiFi credentials:
     ```cpp
     #define WIFI_SSID "your_wifi_name"
     #define WIFI_PASSWORD "your_wifi_password"
     ```
   - Add your Firebase credentials:
     ```cpp
     #define FIREBASE_HOST "your-project.firebaseio.com"
     #define FIREBASE_AUTH "your_database_secret"
     ```

3. **Upload Code**
   - Open `water_quality_monitor.ino` in Arduino IDE
   - Select ESP32 Dev Module board
   - Choose correct COM port
   - Upload the code

## 💻 Usage

1. Power on the device
2. Wait for WiFi connection (indicated on LCD)
3. Place sensors in water sample
4. View real-time readings on LCD display
5. Check Firebase console for logged data
6. Monitor water quality status
