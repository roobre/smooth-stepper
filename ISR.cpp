#include "Stepper.h"
#include "ISR.h"

// ISR deploys timer interrupt routines for the steppers passed in begin()
namespace ISR {
Stepper* steppers;
size_t steppers_len;

void begin(Stepper* steppers, size_t len) {
    ISR::steppers = steppers;
    ISR::steppers_len = steppers_len;

    // Timer initialization. See the following url for details
    // Timer1 interrupt calls Stepper::step(), while Timer3 calls Stepper::accelerate()
    // http://www.8bit-era.cz/arduino-timer-interrupts-calculator.html

    // TIMER 1 for interrupt frequency 20000 Hz:
    cli(); // stop interrupts
    TCCR1A = 0; // set entire TCCR1A register to 0
    TCCR1B = 0; // same for TCCR1B
    TCNT1  = 0; // initialize counter value to 0
    // set compare match register for 20000 Hz increments
    OCR1A = 799; // = 16000000 / (1 * 20000) - 1 (must be <65536)
    // turn on CTC mode
    TCCR1B |= (1 << WGM12);
    // Set CS12, CS11 and CS10 bits for 1 prescaler
    TCCR1B |= (0 << CS12) | (0 << CS11) | (1 << CS10);
    // enable timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);

    // TIMER 3 for interrupt frequency 30.00120004800192 Hz:
    TCCR3A = 0; // set entire TCCR1A register to 0
    TCCR3B = 0; // same for TCCR1B
    TCNT3  = 0; // initialize counter value to 0
    // set compare match register for 30.00120004800192 Hz increments
    OCR3A = 8332; // = 16000000 / (64 * 30.00120004800192) - 1 (must be <65536)
    // turn on CTC mode
    TCCR3B |= (1 << WGM12);
    // Set CS12, CS11 and CS10 bits for 64 prescaler
    TCCR3B |= (0 << CS12) | (1 << CS11) | (1 << CS10);
    // enable timer compare interrupt
    TIMSK3 |= (1 << OCIE1A);
    sei(); // allow interrupts
}

void enable() {
    TIMSK1 |= (1 << OCIE1A);
    TIMSK3 |= (1 << OCIE1A);
}

void disable() {
    TIMSK1 |= (0 << OCIE1A);
    TIMSK3 |= (0 << OCIE1A);
}
}

ISR(TIMER1_COMPA_vect, ISR_BLOCK) {
    for (size_t i = 0; i < ISR::steppers_len; i++) {
        ISR::steppers[i].step();
    }
}

ISR(TIMER3_COMPA_vect, ISR_BLOCK) {
    for (size_t i = 0; i < ISR::steppers_len; i++) {
        ISR::steppers[i].accelerate();
    }
}

