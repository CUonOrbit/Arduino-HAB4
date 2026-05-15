#include "SDCardModule.h"
#include <FlashStorage_SAMD.h>

static File mainFile;
static File backupFile;
static File eventFile;
static bool sdReady = false;

// Flash-backed boot counter, persists across resets
FlashStorage(bootCountStorage, uint8_t);

static String padTwo(unsigned long val) {
    return (val < 10 ? "0" : "") + String(val);
}

String getTimestamp() {
    unsigned long t = millis();
    unsigned long h = (t / 3600000UL) % 24;
    unsigned long m = (t / 60000UL)   % 60;
    unsigned long s = (t / 1000UL)    % 60;
    return String(h) + ":" + padTwo(m) + ":" + padTwo(s);
}

bool initSDCard(uint8_t csPin) {
    Serial.print(F("Attempting SD init on CS pin: "));
    Serial.println(csPin);
    
    if (!SD.begin(csPin)) {
        Serial.println(F("SD init failed!"));
        return false;
    }
    Serial.println(F("SD.begin() succeeded"));

    uint8_t bootCount;
    Serial.println(F("Reading boot count from flash..."));
    bootCountStorage.read(bootCount);
    Serial.print(F("Boot count read: "));
    Serial.println(bootCount);
    
    if (bootCount == 255) bootCount = 0;
    bootCount++;
    
    Serial.print(F("Incrementing to: "));
    Serial.println(bootCount);
    bootCountStorage.write(bootCount);
    Serial.println(F("Boot count written to flash"));

    String mainName   = "D" + String(bootCount) + ".csv";  // D1.csv (data)
    String backupName = "B" + String(bootCount) + ".csv";  // B1.csv (backup)
    String eventName  = "E" + String(bootCount) + ".txt";  // E1.txt (events)

    Serial.print(F("Opening main file: "));
    Serial.println(mainName);
    mainFile   = SD.open(mainName,   FILE_WRITE);
    if (!mainFile) {
        Serial.println(F("FAILED to open main file"));
        return false;
    }
    Serial.println(F("Main file opened"));

    Serial.print(F("Opening backup file: "));
    Serial.println(backupName);
    backupFile = SD.open(backupName, FILE_WRITE);
    if (!backupFile) {
        Serial.println(F("FAILED to open backup file"));
        return false;
    }
    Serial.println(F("Backup file opened"));

    Serial.print(F("Opening event file: "));
    Serial.println(eventName);
    eventFile  = SD.open(eventName,  FILE_WRITE);
    if (!eventFile) {
        Serial.println(F("FAILED to open event file"));
        return false;
    }
    Serial.println(F("Event file opened"));

    Serial.println(F("Writing CSV headers..."));
    String header = "H:M:S,Temp(C),Pressure(hPa),Altitude(m),AccelX,AccelY,AccelZ,GyroX,GyroY,GyroZ";
    mainFile.println(header);
    backupFile.println(header);
    Serial.println(F("Headers written"));

    sdReady = true;
    Serial.println(F("sdReady set to true"));

    logEvent("BOOT #" + String(bootCount) + " - System initialized");

    Serial.print(F("Logging to: "));
    Serial.println(mainName);

    return true;
}

void logData(const String &data) {
    if (!sdReady) return;
    mainFile.println(data);
    backupFile.println(data);
}

void logEvent(const String &message) {
    if (!eventFile) {
        Serial.println(message);
        return;
    }
    String entry = "[" + getTimestamp() + "] " + message;
    eventFile.println(entry);
    eventFile.flush();
    Serial.println(entry);
}

void flushSD() {
    if (!sdReady) return;
    mainFile.flush();
    backupFile.flush();
}

void closeSD() {
    if (!sdReady) return;
    logEvent("SYSTEM SHUTDOWN - closing logs");
    mainFile.flush();   mainFile.close();
    backupFile.flush(); backupFile.close();
    eventFile.flush();  eventFile.close();
    sdReady = false;
}