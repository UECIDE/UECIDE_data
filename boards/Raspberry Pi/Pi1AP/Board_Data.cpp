#include <Arduino.h>

const uint8_t digitalPinToGPIO[DIGITAL_PINS] = {
    NOT_A_PIN,
    NOT_A_PIN,
    18,
    23,
    24,
    25,
    4,
    17,
    21,
    22,
    8,
    10,
    9,
    11,
    7
};

void boardAnalogInit() {
    pinMode(CS1, OUTPUT);
    digitalWrite(CS1, HIGH);
}

uint16_t boardAnalogRead(uint8_t pin) {
    if (pin > 5) return 0;

    uint8_t addr = pin << 3;

    boolean willStop = false;
    if (!CoreSPI.isRunning()) {
        CoreSPI.begin();    
        willStop = true;
    }
    CoreSPI.saveState();
    CoreSPI.setClockDivider(SPI_CLOCK_DIV256);
    digitalWrite(CS1, LOW);
    uint8_t b1 = CoreSPI.transfer(addr);
    uint8_t b2 = CoreSPI.transfer(0);
    digitalWrite(CS1, HIGH);
    CoreSPI.restoreState();
    if (willStop) {
        CoreSPI.end();
    }
    uint16_t bo = (b1 << 6) | (b2 >> 2);
    return bo;
}
