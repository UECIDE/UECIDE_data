#ifndef _GPIO_H
#define _GPIO_H

#include <Arduino.h>

#define GPIO_BASE (IOBASE + 0x200000)

static const uint8_t FSEL_INPT  = 0b000;
static const uint8_t FSEL_OUTP  = 0b001;
static const uint8_t FSEL_ALT0  = 0b100;
static const uint8_t FSEL_ALT1  = 0b101;
static const uint8_t FSEL_ALT2  = 0b110;
static const uint8_t FSEL_ALT3  = 0b111;
static const uint8_t FSEL_ALT4  = 0b011;
static const uint8_t FSEL_ALT5  = 0b010;

class _GPIO {
    private:
        volatile struct bcm2835_peripheral gpio;
        volatile struct bcm2835_gpio *reg;

    public:
        _GPIO(uint32_t);
        void begin();
        void selectFunction(uint8_t, uint8_t);
        void pinMode(uint8_t, uint8_t);
        void digitalWrite(uint8_t, uint8_t);
        uint8_t digitalRead(uint8_t);
        uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);
        void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);
        uint32_t pulseIn(uint8_t pin, uint8_t value, uint32_t timeout = 0);

};

extern _GPIO GPIO;

#endif
