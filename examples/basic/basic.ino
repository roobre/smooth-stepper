#include "Stepper.h"

// Enable, pulse and direction pins for driver
const int ENA = 27;
const int PUL = 23;
const int DIR = 25;

// Set this to GND to stop the motor.
const int ENA_USER = 12;

// Microstep settings
const unsigned char DIVIDER = 4;

// Motor acceleration
const unsigned long ACCEL = 300; // rpm / s

// Angular velocity
const unsigned long RPM = 200;

// Instantiate the library with the pulse pin as parameter
// Note: This does *NOT* set the mode of the pin passed as parameter
Stepper stepper(PUL);

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(PUL, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(ENA_USER, INPUT);
  
  stepper.microsteps = 4;
  stepper.accelRpm(ACCEL);

  digitalWrite(ENA, HIGH);
  digitalWrite(DIR, HIGH);
  digitalWrite(ENA_USER, HIGH);
}

void loop() {
  if (digitalRead(ENA_USER))
    if (millis() < 3e3 || millis() > 6e3)
      // Set the target RPM. The stepper will accelerate smoothly to this speed.
      stepper.setRpm(150);
    else
      // Subsequent calls to setRpm with the same value are (mostly) no-op (setRpm is idempotent)
      stepper.setRpm(300);
  else
    stepper.setRpm(0);

  // stepper.step() must be called each cycle, as frequently as possible.
  stepper.step();
}
