// config.h - Pin Definitions and Hardware Settings

#pragma once
    

// I2C Pins - fixed on Uno
#define SDA_PIN    A4           
#define SCL_PIN    A5         

// SD Card Module (SPI Pins - fixed on Uno)
#define SD_CS_PIN         8           
// (MOSI=11, MISO=12, SCK=13 are fixed in SPI)

#define THERMISTOR_PIN    A0

#define TERMINATION_TIME  3600000UL    // 1 hour in miliseconds; flight for an hour
//#define TERMINATION_TIME  300000UL      // termination test with 10 seconds period = 10000
#define TERMINATION_CUT_TIME  60000UL    // burning Nicr for 1 min = 60000

#define SEA_LEVEL_PRESSURE_HPA 1013.25f

#define BMP_STATUS_LED_PIN 4
#define SD_CARD_STATUS_LED_PIN 5
#define ACCELEROMETER_STATUS_LED_PIN 6
#define MISC_STATUS_LED_PIN 7

#define POLLING_INTERVAL_MS 5000UL // 5Hz polling for sensors