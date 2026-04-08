#pragma once

#include "I2Cdev.h"
#include "MPU6050.h"

struct MPU6050Reading {
  unsigned long timestampMs; // Time since boot when sample was taken
  int16_t ax_raw, ay_raw, az_raw; // raw accelerometer output
  int16_t gx_raw, gy_raw, gz_raw; // raw gyroscope output
  float ax, ay, az; // values with actual units
  float gx, gy, gz;
};

class MPU6050Module {
  public:
    MPU6050Module(uint8_t i2cAddress = 0x68,
                  unsigned long sampleIntervalMs = 200UL);

    // Call once in setup, returns true if the sensor is found and configured.
    bool begin();

    // Call often in loop, returns true when a new reading is available.
    bool update();

    const MPU6050Reading &getLastReading() const;

    void setSampleIntervalMs(unsigned long intervalMs);

  private:
    MPU6050 mpu_;
    MPU6050Reading lastReading_;

    uint8_t i2cAddress_;
    unsigned long sampleIntervalMs_;
    unsigned long lastSampleMs_;
    bool initialized_;
};