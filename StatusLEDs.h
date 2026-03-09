#pragma once
#include <Arduino.h>

/*
* StatusLEDs.h - Simple functions to manage status LEDs for sensor and SD card status.
* 
* The LED for each module being ON, indicates the module is funnctional and operating correctly. 
* If the LED is OFF, it indicates a problem with that module (e.g. sensor not responding, SD card error).
*/


void initStatusLEDs(int pins[], int numPins);
void setStatusLED(int pin, bool status);