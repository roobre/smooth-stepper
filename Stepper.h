#pragma once
#include <stdint.h>

// Stepper class controls pulses so steppers are run smoothly and with configurable acceleration

class Stepper {
public:
    Stepper();
    Stepper(int pulsePin);
    // Step issues a pulse if it is needed
    // This function should be called as frequently as possible (every ~100us or less)
    void step();
    // Accelerate recomputes current period based on accelRpm value
    // This function should be called frequently, but not as much as step.
    // As accelerate only recomputes speed every ACCEL_DELTA_TIME_MIN ms, calling it every ACCEL_DELTA_TIME_MIN ms is enough
    void accelerate();
    // setRpm set the target speed idempotently (can be called over and over with the same value)
    // if set to less than MIN_RPM, stops the motor smoothly but completely
    void setRpm(float rpm);
    // accelRpm sets the acceleration value, in rev/min/s
    void accelRpm(float rpmps);

    // currentSpeed returns the current period (doh)
    float currentSpeed();

    // pulsePin stores which pin should be flipped
    int pulsePin;

    // microsteps is used to adjust pulses/sec to rpm, according to setting in driver
    uint8_t microsteps;
    // pulseWidth sets the time which pulsePin is held up. Most drivers are happy with 2<x<10 us
    // WARNING: holding pulsePin up is BLOCKING (done with delayMicroseconds). Setting this value too high
    //  can and WILL have negative consequences in performance. It should ALWAYS be one, or even two orders of magnitude
    //  smaller than period
    uint8_t pulseWidth;

protected:
    // Inline function which converts rpm to pulses per second
    static uint32_t rpm2pps(float rpm, uint8_t microsteps);
    // Inline function which rpms to period
    static uint32_t rpm2period(float rpm, uint8_t microsteps);
    // Inline function which converts pulses per second to period
    static uint32_t pps2period(uint32_t pps);

    // ComputeAccelTime sets the time which acceleration will take
    void computeAccelTime();

    // Current (instant) period
    uint32_t period;
    // Target period (corresponding to the value set by setRpm)
    uint32_t targetPeriod;
    // Target speed in pulses per seconds
    int32_t targetSpeed;

    int32_t accelBeginSpeed;
    uint32_t accel;
    int32_t accelTime;
    uint16_t deltaAccelTime;

    uint32_t lastMicros;
    uint32_t beginAccelMillis;

    bool accelerating;
    bool stopRequested;

    static const float MIN_RPM;
    static const uint8_t DEFAULT_PULSEWIDTH = 10;
    static const uint8_t ARDUINO_PULSEWIDTH_OFFSET = 2;
    static const uint16_t DEFAULT_ACCEL = 200;
    static const uint16_t START_RPM = 10;
    static const uint8_t ACCEL_DELTA_TIME_MIN = 5;
};

