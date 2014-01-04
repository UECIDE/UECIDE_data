#include <Arduino.h>
#include <sys/time.h>

_GPIO GPIO(GPIO_BASE);

_GPIO::_GPIO(uint32_t addr) {
    gpio.addr_p = addr;
}

void _GPIO::begin() {
    map_peripheral(&gpio);
    reg = (bcm2835_gpio *)gpio.map;

    for (int i = 0; i < DIGITAL_PINS; i++) {
        selectFunction(i, FSEL_INPT);
    }

    sync_peripheral(&gpio);
}

void _GPIO::selectFunction(uint8_t pin, uint8_t func) {
    if (pin >= DIGITAL_PINS) { return; }
    pin = raspberryPinNumber(pin);
    if (pin == NOT_A_PIN) { return; } 

    uint8_t bank = pin / 10;
    uint8_t slot = pin % 10;

    switch(slot) {
        case 0: 
            reg->gpfsel[bank].fsel0 = 0; 
            sync_peripheral(&gpio);
            reg->gpfsel[bank].fsel0 = (func & 0b111); 
            sync_peripheral(&gpio);
        break;
        case 1: 
            reg->gpfsel[bank].fsel1 = 0; 
            sync_peripheral(&gpio);
            reg->gpfsel[bank].fsel1 = (func & 0b111); 
            sync_peripheral(&gpio);
        break;
        case 2: 
            reg->gpfsel[bank].fsel2 = 0; 
            sync_peripheral(&gpio);
            reg->gpfsel[bank].fsel2 = (func & 0b111); 
            sync_peripheral(&gpio);
        break;
        case 3: 
            reg->gpfsel[bank].fsel3 = 0; 
            sync_peripheral(&gpio);
            reg->gpfsel[bank].fsel3 = (func & 0b111); 
            sync_peripheral(&gpio);
        break;
        case 4: 
            reg->gpfsel[bank].fsel4 = 0; 
            sync_peripheral(&gpio);
            reg->gpfsel[bank].fsel4 = (func & 0b111); 
            sync_peripheral(&gpio);
        break;
        case 5: 
            reg->gpfsel[bank].fsel5 = 0; 
            sync_peripheral(&gpio);
            reg->gpfsel[bank].fsel5 = (func & 0b111); 
            sync_peripheral(&gpio);
        break;
        case 6: 
            reg->gpfsel[bank].fsel6 = 0; 
            sync_peripheral(&gpio);
            reg->gpfsel[bank].fsel6 = (func & 0b111); 
            sync_peripheral(&gpio);
        break;
        case 7: 
            reg->gpfsel[bank].fsel7 = 0; 
            sync_peripheral(&gpio);
            reg->gpfsel[bank].fsel7 = (func & 0b111); 
            sync_peripheral(&gpio);
        break;
        case 8: 
            reg->gpfsel[bank].fsel8 = 0; 
            sync_peripheral(&gpio);
            reg->gpfsel[bank].fsel8 = (func & 0b111); 
            sync_peripheral(&gpio);
        break;
        case 9: 
            reg->gpfsel[bank].fsel9 = 0;
            sync_peripheral(&gpio);
            reg->gpfsel[bank].fsel9 = (func & 0b111); 
            sync_peripheral(&gpio);
        break;
    }
}

void _GPIO::pinMode(uint8_t pin, uint8_t mode) {
    switch (mode) {
        case OUTPUT:
            selectFunction(pin, FSEL_OUTP);
            break;
        case INPUT:
            selectFunction(pin, FSEL_INPT);
            break;
    }
}

void _GPIO::digitalWrite(uint8_t pin, uint8_t level) {
    if (pin >= DIGITAL_PINS) { return; }
    pin = raspberryPinNumber(pin);
    if (pin == NOT_A_PIN) { return; } 

    uint8_t bank = pin / 32;
    uint8_t slot = pin % 32;

    if (level == LOW) {
        reg->gpclr[bank] = 1 << slot;
    } else {
        reg->gpset[bank] = 1 << slot;
    }
    sync_peripheral(&gpio);
}

uint8_t _GPIO::digitalRead(uint8_t pin) {
    if (pin >= DIGITAL_PINS) { return 0; } 
    pin = raspberryPinNumber(pin); 
    if (pin == NOT_A_PIN) { return 0; } 

    uint8_t bank = pin / 32;
    uint8_t slot = pin % 32;

    return (reg->gplev[bank] & (1<<slot)) ? HIGH : LOW;
}
    

uint8_t _GPIO::shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {
    uint8_t value = 0;
    uint8_t i;

    for (i = 0; i < 8; ++i) {
        digitalWrite(clockPin, HIGH);
        if (bitOrder == LSBFIRST)
            value |= digitalRead(dataPin) << i;
        else
            value |= digitalRead(dataPin) << (7 - i);
        digitalWrite(clockPin, LOW);
    }
    return value;
}

void _GPIO::shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val) {
    uint8_t i;

    for (i = 0; i < 8; i++)  {
        if (bitOrder == LSBFIRST)
            digitalWrite(dataPin, !!(val & (1 << i)));
        else
            digitalWrite(dataPin, !!(val & (1 << (7 - i))));

        digitalWrite(clockPin, HIGH);
        digitalWrite(clockPin, LOW);
    }
}

uint32_t _GPIO::pulseIn(uint8_t pin, uint8_t value, uint32_t timeout) {
    struct timeval tv;
    struct timezone tz;
    unsigned long long start = 0;
    unsigned long long now;

    if (pin >= DIGITAL_PINS) { return 0; } 
    pin = raspberryPinNumber(pin); 
    if (pin == NOT_A_PIN) { return 0; } 

    uint8_t bank = pin / 32;
    uint8_t slot = pin % 32;

    tz.tz_minuteswest = 0;
    tz.tz_dsttime = 0;

    uint32_t match = (value & 1) << slot;
    uint32_t mask = 1 << slot;

    gettimeofday(&tv, &tz);
    start = tv.tv_sec * 1000000UL + tv.tv_usec;

    while ((reg->gplev[bank] & mask) != match) {
        gettimeofday(&tv, &tz);
        now = tv.tv_sec * 1000000UL + tv.tv_usec;
        if ((timeout > 0) && ((now - start) > timeout)) {
            return 0;
        }
        sync_peripheral(&gpio);
    }
    gettimeofday(&tv, &tz);
    start = tv.tv_sec * 1000000UL + tv.tv_usec;
    while ((reg->gplev[bank] & mask) == match) {
        gettimeofday(&tv, &tz);
        now = tv.tv_sec * 1000000UL + tv.tv_usec;
        if ((timeout > 0) && ((now - start) > timeout)) {
            return 0;
        }
        sync_peripheral(&gpio);
    }
    return (now - start);
} 
