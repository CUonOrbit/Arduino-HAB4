# HAB BMP280 Data Logger 🚀

[![Arduino](https://img.shields.io/badge/platform-Arduino-blue.svg)](https://www.arduino.cc/)

**High Altitude Balloon (HAB) telemetry payload** featuring **5Hz BMP280 logging** to SD card with **autonomous cut-down** at **30-minute timeout**.

## ✨ Features

- **5Hz BMP280 logging** (temperature, pressure, altitude)
- **32GB SD card** data storage with timestamps
- **Time termination**: 30 minutes flight time
- **Standalone operation** (9V lithium battery)
- **Out-of-range detection** (>9km altitude warning)
- **Arduino Nano 33 IoT** compatible

## 🛠️ Hardware Requirements

### Required:
| Component | Quantity |
|-----------|----------|
| [BMP280 barometer](https://www.adafruit.com/product/2651) (I2C) | 1 |
| SD card adapter + **32GB microSD** (FAT32) | 1 |
| **9V lithium battery** | 1 |
| [Relay module](https://www.amazon.com/s?k=5v+relay+module) (cut-down) | 1 |

### Wiring Diagram

| Component | VCC | GND | SDI/SDA | CS | SCK/SCL | MOSI | MISO |
|-----------|-----|-----|---------|----|---------|------|------|
| **BMP280** | **5V** | **GND** | **A4** | - | **A5** | - | - |
| **MPU6050** | **5V** | **GND** | **A4** | - | **A5** | - | - |
| **SD Card** | **5V** | **GND** | - | **D8** | **D13** | **D11** | **D12** |
| **BMP_STATUS_LED_PIN** | **D4** | **GND** | - | - | - | - | - |
| **SD_CARD_STATUS_LED_PIN** | **D5** | **GND** | - | - | - | - | - |
| **ACCELEROMETER_STATUS_LED_PIN** | **D6** | **GND** | - | - | - | - | - |
| **SYSTEM_LED_PIN** | **D7** | **GND** | - | - | - | - | - |


## 🚀 Installation
1. Install Libraries (Arduino IDE Library Manager):
- Adafruit BMP280 Library
- Adafruit Unified Sensor
- SD (built-in)
- MPU6050 by Electronic Cats
- FlashStorage_SAMD by Cristian Maglie


3. Upload Arduino-HAB3.ino to Arduino Nano 33 IoT

4. Insert FAT32 formatted 32GB microSD card

Power:

- Standalone: 9V lithium → Arduino 9V DC power barrel jack

- PC Test: USB only

### 🔄 Operation Flow

1. Power ON → BMP280 + SD init + Thermistor
2. 5Hz logging → Data → SD card
3. Every 5s → Buffer flush (safety)


### 🆘 Troubleshooting
| Symptom              | Cause             | Fix                              |
| -------------------- | ----------------- | -------------------------------- |
| "BMP280 init failed" | Wrong I2C address | (0x76)                           |
| "SD init failed"     | Wrong CS pin      | Verify D4 connection             |
| No telemetry.csv     | SD not formatted  | Verify pin connection            |
| Hangs at startup     | while(!Serial)    | Comment out for standalone       |

## 📈 Development Status
- ✅ BMP280 5Hz logging
- ✅ SD card storage
- ✅ Autonomous termination
- ✅ Standalone operation
- 🔄 GPS/AHT10 integration (future)

## ⚙️ Pin Configuration (`config.h`)

```cpp
#define SD_CS_PIN          8
#define THERMISTOR_PIN    A0
