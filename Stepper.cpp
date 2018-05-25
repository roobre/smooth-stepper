#include <Arduino.h>
#include <stdint.h>
#include "Stepper.h"

#define SERIALVAR(x) Serial.print(#x ": "); Serial.println(x)

const float Stepper::MIN_RPM = 1.0;

Stepper::Stepper() {
    microsteps = 4;
    pulseWidth = DEFAULT_PULSEWIDTH;
    period = rpm2period(START_RPM, microsteps);
    targetPeriod = period;
    accelerating = false;
    stopRequested = true;
}

Stepper::Stepper(int pulsePin) : Stepper() {
    this->pulsePin = pulsePin;
}

inline uint32_t Stepper::rpm2pps(float rpm, uint8_t microsteps) {
    return ceil(rpm * 360 * microsteps / 1.8 / 60);
}

inline uint32_t Stepper::pps2period(uint32_t pps) {
    return 1e6 / pps;
}

inline uint32_t Stepper::rpm2period(float rpm, uint8_t microsteps) {
    return pps2period(rpm2pps(rpm, microsteps));
}

void Stepper::setRpm(float rpm) {
    stopRequested = rpm < MIN_RPM;
    targetSpeed = rpm2pps(stopRequested ? MIN_RPM : rpm, microsteps);
    uint32_t nTargetPeriod = pps2period(targetSpeed);

    if (abs(nTargetPeriod - targetPeriod) > 2) {
        accelerating = true;
        accelBeginSpeed = pps2period(period);
        targetPeriod = nTargetPeriod;
        beginAccelMillis = millis(); // Substract 5 to avoid corner case
        computeAccelTime();
    }
}

void Stepper::accelRpm(float rpmps) {
    accel = rpm2pps(rpmps, microsteps);
    computeAccelTime();
}

void Stepper::computeAccelTime() {
    accelTime = abs(targetSpeed - accelBeginSpeed) * 1000 / accel;
}

float Stepper::currentSpeed() {
    // return this->targetSpeed * 1.8 * 60 / 360 / microsteps;
    return this->period;
}

void Stepper::accelerate() {
    if (__builtin_expect(accelerating && (deltaAccelTime = millis() - beginAccelMillis) > ACCEL_DELTA_TIME_MIN, false)) {
        // TODO: Try to conditionally cast into 32/16/8b periods.
        uint32_t newPeriod = pps2period(accelBeginSpeed + (targetSpeed - accelBeginSpeed) * deltaAccelTime / accelTime);

        if ((accelBeginSpeed < targetSpeed && newPeriod <= targetPeriod) || (accelBeginSpeed > targetSpeed && newPeriod >= targetPeriod)) {
            period = targetPeriod;
            accelerating = false;
        } else {
            period = newPeriod;
        }
    }
}

void Stepper::step() {
    if (!(stopRequested  && !accelerating) && micros() - lastMicros >= period - ARDUINO_PULSEWIDTH_OFFSET) {
        //if (!(!(micros() - lastMicros >= period - ARDUINO_PULSEWIDTH_OFFSET) || !(!accelerating && stopRequested))) {
        lastMicros = micros(); // Update first, ask later.
        digitalWrite(pulsePin, HIGH);
        delayMicroseconds(pulseWidth - ARDUINO_PULSEWIDTH_OFFSET);
        digitalWrite(pulsePin, LOW);
    }
}
