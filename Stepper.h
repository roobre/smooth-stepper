#pragma once

#include <stdint.h>

//#define TELEMETRY

class Stepper {
public:
    Stepper(int pulsePin);
    void step();
    void setRpm(float rpm);
    void accelRpm(float rpmps);

    int pulsePin;

    uint8_t microsteps;
    uint8_t pulseWidth;

protected:
    uint32_t rpm2pps(float rpm);
    uint32_t rpm2period(float rpm);
    uint32_t pps2period(uint32_t pps);

    void computeAccelTime();

    uint32_t period;
    uint32_t targetPeriod;
    uint32_t targetSpeed;

    uint32_t accelBeginSpeed;
    uint32_t accel;
    int32_t accelTime;
    uint16_t deltaAccelTime;

    uint32_t lastMicros;
    uint32_t beginAccelMillis;

    bool accelerating;
    bool stopRequested;

    static const float MIN_RPM;
    static const uint8_t DEFAULT_PULSEWIDTH = 100;
    static const uint8_t ARDUINO_PULSEWIDTH_OFFSET = 2;
    static const uint16_t DEFAULT_ACCEL = 200;
    static const uint16_t START_RPM = 10;
    static const uint8_t ACCEL_DELTA_TIME_MIN = 5;
};

