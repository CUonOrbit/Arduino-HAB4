#include <Wire.h>
#include "SDCardModule.h"
#include "BMP280Module.h"
#include "MPU6050Module.h"
#include "config.h"
#include "StatusLEDs.h"

bool pulseStarted = false;  // Termination
unsigned long startTime = 0;

// LED blink timing
const int errorBlinkInterval = 500; // milliseconds
bool errorLedState = false;
unsigned long lastBlinkTime = 0;

unsigned long lastSensorMs = 0;

// flush every 10 seconds
const unsigned long FLUSH_INTERVAL_MS = 10000UL; 
unsigned long lastFlushMs = 0;  

// Create module instance
BMP280Module bmp280(0x77, 1013.25f, POLLING_INTERVAL_MS);
MPU6050Module mpu6050(0x68, POLLING_INTERVAL_MS);

// Status LED flags
bool sdCardOK = false;
bool bmp280OK = false;
bool mpu6050OK = false; 

int statusPins[] = {BMP_STATUS_LED_PIN, SD_CARD_STATUS_LED_PIN, ACCELEROMETER_STATUS_LED_PIN};

void setup() {
  // REMINDER:
  // Resetting or disconnecting/reconnecting power
  // restarts the 1-hour timer from zero.
  pinMode(RELAY_PIN, OUTPUT);          // NiCr output
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  Serial.begin(115200);
  //while (!Serial);      // remove when standalone
  initStatusLEDs(statusPins, 3);
  delay(1000);

  sdCardOK = initSDCard(SD_CS_PIN);
  bmp280OK = bmp280.begin();
  mpu6050OK = mpu6050.begin();

  if (!bmp280OK)  logEvent("ERROR: BMP280 init failed");
  if (!mpu6050OK) logEvent("ERROR: MPU6050 init failed");
  if (!sdCardOK)  Serial.println(F("ERROR: SD init failed"));

  logEvent("All systems go - beginning data collection");
  if (!sdCardOK || !bmp280OK || !mpu6050OK) {
    errorLedState = true;
  }

  Serial.println("H:M:S,Temp,Pressure,Altitude,BoardTemp");
  startTime = millis(); // Start the 1-hour countdown
  lastSensorMs = millis();
  lastFlushMs = millis();
}

void loop(){
  unsigned long now = millis();

  // Error LED handling
  digitalWrite(LED_BUILTIN, HIGH);
  setStatusLED(BMP_STATUS_LED_PIN, bmp280OK);
  setStatusLED(SD_CARD_STATUS_LED_PIN, sdCardOK);
  setStatusLED(ACCELEROMETER_STATUS_LED_PIN, mpu6050OK);

  // 5Hz sensor logging 
  if (now - lastSensorMs >= POLLING_INTERVAL_MS) {
    lastSensorMs += POLLING_INTERVAL_MS;

    String timestamp = getTimestamp();
    String bmpPart = "";
    String mpuPart = "";

    // BMP280 data
    if (bmp280.update()) {
      const BMP280Reading &rbmp = bmp280.getLastReading();
      if (!bmp280.isValid()) {
        bmpPart = String(rbmp.temperatureC) + "," +
                  String(rbmp.pressureHpa) + "(OOR)," +
                  String(rbmp.altitudeM) + "(OOR)";
      } else {
        bmpPart = String(rbmp.temperatureC) + "," +
                  String(rbmp.pressureHpa) + "," +
                  String(rbmp.altitudeM);
      }
    }

    // MPU6050 data
    if (mpu6050.update()) {
      const MPU6050Reading &rmpu = mpu6050.getLastReading();
      mpuPart = String(rmpu.ax) + "," + String(rmpu.ay) + "," + String(rmpu.az) + "," +
                String(rmpu.gx) + "," + String(rmpu.gy) + "," + String(rmpu.gz);
    }

    String row = timestamp + "," + bmpPart + "," + mpuPart;
    logData(row);
    Serial.println(row);

    // Periodic SD flush
    if (now - lastFlushMs >= FLUSH_INTERVAL_MS) {
      lastFlushMs += FLUSH_INTERVAL_MS;
      flushSD();
    }

    // --- Step 1: Wait 1 hour ---
    if (!pulseStarted && (millis() - startTime >= TERMINATION_TIME)) {
      logEvent("TERMINATING FLIGHT");
      digitalWrite(RELAY_PIN, HIGH);
      pulseStarted = true;
      startTime = millis();
    }

    // --- Step 2: Run NiCr for 1 minute then off ---
    if (pulseStarted && (millis() - startTime >= TERMINATION_CUT_TIME)) {
      digitalWrite(RELAY_PIN, LOW);
      logEvent("FLIGHT TERMINATION COMPLETE");
    }
  }
}