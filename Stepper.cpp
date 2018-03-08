#include <Arduino.h>
#include <stdint.h>
#include "Stepper.h"

#define SERIALVAR(x) Serial.print(#x ": "); Serial.println(x)

const float Stepper::MIN_RPM = 1.0;

Stepper::Stepper(int pulsePin) {
    this->pulsePin = pulsePin;
    microsteps = 1;
    pulseWidth = DEFAULT_PULSEWIDTH;
    period = rpm2period(START_RPM);
    targetPeriod = period;
    accelerating = false;
    stopRequested = true;
}

inline uint32_t Stepper::rpm2pps(float rpm) {
    return ceil(rpm * 360 * microsteps / 1.8 / 60);
}

inline uint32_t Stepper::pps2period(uint32_t pps) {
    return 1e6 / pps;
}

inline uint32_t Stepper::rpm2period(float rpm) {
    return pps2period(rpm2pps(rpm));
}

void Stepper::setRpm(float rpm) {
    stopRequested = rpm < MIN_RPM;
    targetSpeed = rpm2pps(stopRequested ? MIN_RPM : rpm);
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
    accel = rpm2pps(rpmps);
    computeAccelTime();
}

void Stepper::computeAccelTime() {
    accelTime = abs((int32_t)targetSpeed - (int32_t)accelBeginSpeed) * 1000 / accel;
}

void Stepper::step() {
    if (__builtin_expect(accelerating && (deltaAccelTime = millis() - beginAccelMillis) > ACCEL_DELTA_TIME_MIN, false)) {
        // TODO: Try to conditionally cast into 32/16/8b periods.
        uint32_t newPeriod = pps2period(accelBeginSpeed + ((int32_t)targetSpeed - (int32_t)accelBeginSpeed) * deltaAccelTime / accelTime);

        if ((accelBeginSpeed < targetSpeed && newPeriod <= targetPeriod) || (accelBeginSpeed > targetSpeed && newPeriod >= targetPeriod)) {
            period = targetPeriod;
            accelerating = false;
        } else {
            period = newPeriod;
        }
    }

    if (!(stopRequested  && !accelerating) && micros() - lastMicros >= period - ARDUINO_PULSEWIDTH_OFFSET) {
        //if (!(!(micros() - lastMicros >= period - ARDUINO_PULSEWIDTH_OFFSET) || !(!accelerating && stopRequested))) {
        lastMicros = micros(); // Update first, ask later.
        digitalWrite(pulsePin, HIGH);
        delayMicroseconds(pulseWidth - ARDUINO_PULSEWIDTH_OFFSET);
        digitalWrite(pulsePin, LOW);
    }
}
