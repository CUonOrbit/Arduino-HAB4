#include "MPU6050Module.h"

MPU6050Module::MPU6050Module(uint8_t i2cAddress, 
                            unsigned long sampleIntervalMs)
    : i2cAddress_(i2cAddress),
      sampleIntervalMs_(sampleIntervalMs),
      lastSampleMs_(0),
      initialized_(false) {
    lastReading_.timestampMs = 0;
    lastReading_.ax          = NAN;
    lastReading_.ay          = NAN;
    lastReading_.az          = NAN;
    lastReading_.gx          = NAN;
    lastReading_.gy          = NAN;
    lastReading_.gz          = NAN;
}

bool MPU6050Module::begin() {
    Wire.begin();
    mpu_.initialize();
    if (!mpu_.testConnection()) { 
        initialized_ = false;
        return false;
    }

    lastSampleMs_ = millis();
    initialized_  = true;
    return true;
}

bool MPU6050Module::update() {
    if (!initialized_) {
        return false;
    }

    unsigned long now = millis();

    if (now - lastSampleMs_ < sampleIntervalMs_) {
        return false;                       // not time yet for a new sample
    }

    // Keep schedule stable
    lastSampleMs_ += sampleIntervalMs_;

    mpu_.getMotion6(&lastReading_.ax_raw, &lastReading_.ay_raw, &lastReading_.az_raw, &lastReading_.gx_raw, &lastReading_.gy_raw, &lastReading_.gz_raw);
    lastReading_.timestampMs  = now;
    lastReading_.ax           = lastReading_.ax_raw / 16384.0; // values based on default accelerometer and gyro range
    lastReading_.ay           = lastReading_.ay_raw / 16384.0;
    lastReading_.az           = lastReading_.az_raw / 16384.0;
    lastReading_.gx           = lastReading_.gx_raw / 131.0;
    lastReading_.gy           = lastReading_.gy_raw / 131.0;
    lastReading_.gz           = lastReading_.gz_raw / 131.0;

    return true;
}

const MPU6050Reading &MPU6050Module::getLastReading() const {
    return lastReading_;
}

void MPU6050Module::setSampleIntervalMs(unsigned long intervalMs) {
    sampleIntervalMs_ = intervalMs;
}
