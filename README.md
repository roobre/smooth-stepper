# Smooth Stepper

Smooth Stepper is a library to control stepper motor drivers. It allows the user to set a target angular velocity and an acceleration, and the library takes care of the rest.

## Usage

```cpp
const int ENA = 27;
const int PUL = 23;
const int DIR = 25;

Stepper stepper(PUL);

void setup() {
    pinMode(ENA, OUTPUT);
    pinMode(PUL, OUTPUT);
    pinMode(DIR, OUTPUT);

    stepper.microsteps = 4;
    stepper.setRpm(100); // rpm
    stepper.accelRpm(200); // rpm/s

    digitalWrite(ENA, HIGH);
    digitalWrite(DIR, HIGH);
}

void loop() {
    stepper.step();
    stepper.accelerate();

    if (millis() > 5e3 && millis() < 6e3) {
        // Double the speed after 5 seconds.
        stepper.setRpm(200);
    }
}
```
