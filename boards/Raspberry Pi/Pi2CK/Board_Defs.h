#ifndef _BOARD_DEFS_H
#define _BOARD_DEFS_H

#define DIGITAL_PINS 23

extern const uint8_t digitalPinToGPIO[DIGITAL_PINS];

#define CS0 19
#define CS1 21
#define MISO 218
#define MOSI 16
#define SCK 20

extern void boardAnalogInit();
#define ANALOG_INIT boardAnalogInit

extern uint16_t boardAnalogRead(uint8_t);
#define ANALOG_READ boardAnalogRead


#endif
