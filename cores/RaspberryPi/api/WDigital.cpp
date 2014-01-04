#include <Arduino.h>

// These are just stubs that redirect to the GPIO class instance.

void pinMode(uint8_t pin, uint8_t mode) {
    PWM.release(pin);
    GPIO.pinMode(pin, mode);
}

void digitalWrite(uint8_t pin, uint8_t value) {
    GPIO.digitalWrite(pin, value);
}

uint8_t digitalRead(uint8_t pin) {
    return GPIO.digitalRead(pin);
}

uint32_t pulseIn(uint8_t pin, uint8_t value) {
    return pulseIn(pin, value, 0);
}

uint32_t pulseIn(uint8_t pin, uint8_t value, uint32_t timeout) {
    return GPIO.pulseIn(pin, value, timeout);
}

