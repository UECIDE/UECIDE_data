#ifndef _WTYPES_H
#define _WTYPES_H

#include <stdint.h>

typedef bool boolean;
typedef uint8_t byte;

static const uint8_t NOT_A_PIN = 0xFF;

static const uint8_t LOW     = 0x00;
static const uint8_t HIGH    = 0x01;
static const uint8_t RISING  = 0x02;
static const uint8_t FALLING = 0x03;
static const uint8_t BOTH    = 0x04;

static const uint8_t INPUT = 0x00;
static const uint8_t OUTPUT = 0x01;
static const uint8_t OPEN = 0x02;
static const uint8_t INPUT_PULLUP = 0x03;

static const double PI = 3.1415926535897932384626433832795;
static const double HALF_PI = 1.5707963267948966192313216916398;
static const double TWO_PI = 6.283185307179586476925286766559;
static const double DEG_TO_RAD = 0.017453292519943295769236907684886;
static const double RAD_TO_DEG = 57.295779513082320876798154814105;

static const uint8_t LSBFIRST = 0;
static const uint8_t MSBFIRST = 1;

static const uint8_t DEC = 10;
static const uint8_t HEX = 16;
static const uint8_t OCT = 8;
static const uint8_t BIN = 2;
static const uint8_t BYTE = 0;

#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))


#endif
