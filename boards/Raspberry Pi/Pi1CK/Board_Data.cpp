#include <Arduino.h>

#include <Board_Defs.h>

const uint8_t digitalPinToGPIO[DIGITAL_PINS] = {
    NOT_A_PIN,  //  0 - non existant
    NOT_A_PIN,  //  1 - not connected
    0,          //  2 - GPIO 0 / SDA
    1,          //  3 - GPIO 1 / SCL
    NOT_A_PIN,  //  4 - not connected
    4,          //  5 - GPIO 4
    NOT_A_PIN,  //  6 - Ground
    18,         //  7 - GPIO 18
    17,         //  8 - GPIO 17
    NOT_A_PIN,  //  9 - Ground
    21,         // 10 - GPIO 21
    23,         // 11 - GPIO 23
    22,         // 12 - GPIO 22
    24,         // 13 - GPIO 24
    NOT_A_PIN,  // 14 - 3.3V
    NOT_A_PIN,  // 15 - Ground
    10,         // 16 - GPIO 10 / MOSI
    25,         // 17 - GPIO 25
    9,          // 18 - GPIO 9 / MISO
    8,          // 19 - GPIO 8 / CS0
    11,         // 20 - GPIO 11 / SCK
    7,          // 21 - GPIO 7 / CS1
    NOT_A_PIN,  // 22 - Ground
};

void boardAnalogInit() {
}

uint16_t boardAnalogRead(uint8_t pin) {
    return 0;
}
