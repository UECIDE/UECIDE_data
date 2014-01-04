#ifndef _BOARD_DEFS_H
#define _BOARD_DEFS_H

#define DIGITAL_PINS 15
extern const uint8_t digitalPinToGPIO[DIGITAL_PINS];

#define CS0 10
#define CS1 14
#define MISO 12
#define MOSI 11
#define SCK 13

#define A0 0
#define A1 1
#define A2 2
#define A3 3
#define A4 4
#define A5 5


extern void boardAnalogInit();
#define ANALOG_INIT boardAnalogInit

extern uint16_t boardAnalogRead(uint8_t);
#define ANALOG_READ boardAnalogRead

#endif
