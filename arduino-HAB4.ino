#include <Wire.h>
#include "SDCardModule.h"
#include "BMP280Module.h"
#include "ThermistorModule.h"
#include "config.h"
#include "StatusLEDs.h"

bool pulseStarted = false;  // Termination
unsigned long startTime = 0;

// LED blink timing
const int errorBlinkInterval = 500; // milliseconds
bool errorLedState = false;
unsigned long lastBlinkTime = 0;

// 5 Hz -> 200 ms interval for BMP280
const unsigned long SENSOR_INTERVAL_MS = 200UL; // 5Hz
unsigned long lastSensorMs = 0;

// flush every 5 seconds
const unsigned long FLUSH_INTERVAL_MS = 5000UL; 
unsigned long lastFlushMs = 0;

// Create module instance
BMP280Module bmp280(0x77, 1013.25f, SENSOR_INTERVAL_MS);
ThermistorModule thermistor(THERMISTOR_PIN);

// Status LED flags
bool sdCardOK = false;
bool bmp280OK = false;
bool accelerometerOK = false; 

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
  Serial.println(F("Initialize BMP280 5Hz, SD Card"));

  // Track module initialization status
  sdCardOK = initSDCard(SD_CS_PIN);
  bmp280OK = bmp280.begin();


  if (!sdCardOK) Serial.println(F("SD card initialization failed."));
  if (!bmp280OK) Serial.println(F("BMP280 initialization failed."));


  Serial.print("SD Card status: ");
  Serial.println(sdCardOK ? "Success" : "Failed");

  Serial.print("BMP280 status: ");
  Serial.println(bmp280OK ? "Success" : "Failed");

  
  if (!sdCardOK || !bmp280OK) {
    errorLedState = true;
  }

  Serial.println("H:M:S,Temp,Pressure,Altitude,BoardTemp");
  String header_csv = "H:M:S,Temp(°C),Pressure(hPa),Altitude(m),BoardTemp(°C)";
  logToSDCard(header_csv + "\n");
  startTime = millis(); // Start the 1-hour countdown
  lastSensorMs = millis();
  lastFlushMs = millis();
}

void loop(){
  unsigned long now = millis();

  // Error LED handling
  if (errorLedState) {
    digitalWrite(LED_BUILTIN, HIGH);
    setStatusLED(BMP_STATUS_LED_PIN, bmp280OK);
    setStatusLED(SD_CARD_STATUS_LED_PIN, sdCardOK);
    setStatusLED(ACCELEROMETER_STATUS_LED_PIN, accelerometerOK);
  }

  // 5Hz sensor logging 
  if (now - lastSensorMs >= SENSOR_INTERVAL_MS) {
    // Move the timestamp forward by exactly one interval
    lastSensorMs += SENSOR_INTERVAL_MS;

    // BMP280 data
    if (bmp280.update()) {                     // will be true whenever it takes a new sample
      const BMP280Reading &r = bmp280.getLastReading();
      String timestamp = getTimestamp();
      if (!bmp280.isValid()) {                // checks the last reading's pressure against the range (300-1100 hPa)
        String barometerLog = timestamp +
                                "," + r.temperatureC +
                                "," + r.pressureHpa +
                                "(OOR)," + r.altitudeM +       // flag it as out-of-range
                                "(OOR)";  
        logToSDCard(barometerLog);
      } else {
        String barometerLog = timestamp +
                                "," + r.temperatureC +
                                "," + r.pressureHpa +
                                "," + r.altitudeM +
                                ",";
        logToSDCard(barometerLog);                        
      }
    } 

    float boardTempC = thermistor.readTemperatureC();
    if (thermistor.isValid(boardTempC)) {
      String tempLog = thermistor.getLogString(boardTempC);
      logToSDCard(tempLog + "\n");  // Logs to SD + Serial
      Serial.print(","); 
      Serial.print(boardTempC, 1);
      Serial.println(F(""));
    } else {
      logToSDCard("BoardTemp: INVALID\n");
      Serial.println(F("BoardTemp: INVALID"));
    }

    // Periodic SD flush
    if (now - lastFlushMs >= FLUSH_INTERVAL_MS) {
      lastFlushMs += FLUSH_INTERVAL_MS;
      flushSD();
    }
  
    // --- Step 1: Wait 1 hour ---
    if (!pulseStarted && (millis() - startTime >= TERMINATION_TIME))  {
      logToSDCard("TERMINATING FLIGHT");
      digitalWrite(RELAY_PIN, HIGH);            // RELAY_PIN 5; Turn NiCr ON
      pulseStarted = true;              // Mark that the 1-minute pulse has begun
      startTime = millis();             // Reuse timer to measure the 1-minute duration
    }

    // --- Step 2: After pulse starts, run NiCr for 1 minute ---
    if (pulseStarted && (millis() - startTime >= TERMINATION_CUT_TIME))  {
      digitalWrite(RELAY_PIN, LOW);                                            // Turn NiCr OFF after 1 minute
      logToSDCard("FLIGHT TERMINATION COMPLETE");
      // After this point, everything stays off permanently
    }
  }
}