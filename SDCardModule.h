#ifndef SDCARD_MODULE_H
#define SDCARD_MODULE_H

#include <SD.h>
#include <SPI.h>

bool initSDCard(uint8_t csPin = 4);

void logData(const String &data);
void logEvent(const String &message);

void flushSD();
void closeSD();
String getTimestamp();

#endif