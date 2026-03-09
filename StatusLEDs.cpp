#include "StatusLEDs.h"

void initStatusLEDs(int pins[], int numPins)
{
    for (int i = 0; i < numPins; i++)
    {
        pinMode(pins[i], OUTPUT);
    }
}

void setStatusLED(int pin, bool status)
{
    digitalWrite(pin, status ? HIGH : LOW);
}