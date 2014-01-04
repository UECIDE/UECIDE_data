#ifndef _WDIGITAL_H
#define _WDIGITAL_H

extern void pinMode(uint8_t pin, uint8_t mode);
extern void digitalWrite(uint8_t pin, uint8_t level);
extern uint8_t digitalRead(uint8_t pin);
extern uint32_t pulseIn(uint8_t pin, uint8_t value);
extern uint32_t pulseIn(uint8_t pin, uint8_t value, uint32_t timeout);

#endif

