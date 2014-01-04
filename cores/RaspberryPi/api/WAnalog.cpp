#include <Arduino.h>

void analogInit() {
#ifdef ANALOG_INIT
    ANALOG_INIT();
#endif
}

uint16_t analogRead(uint8_t pin) {
#ifdef ANALOG_READ
    return ANALOG_READ(pin);
#endif
    return rand() & 0x3FF;
}

void analogWrite(uint8_t pin, uint8_t val) {
    PWM.analogWrite(pin, val);
}
