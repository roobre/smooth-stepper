#pragma once
#include "Stepper.h"

// ISR deploys timer interrupt routines for the steppers passed in begin()
namespace ISR {
// begin creates and deploys interrupt routines which calls the appropiate methods in steppers passed as parameters
void begin(Stepper* steppers);
// disable disables interrupts
void disable();
// enable activates timer interrupts. This is done by default, there is no need to call enable() if disable() hasn't been called
void enable();
}
