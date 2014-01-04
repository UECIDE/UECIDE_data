#include <CoreSPI.h>

_CoreSPI CoreSPI(BCM2835_SPI0_BASE);

_CoreSPI::_CoreSPI(uint32_t addr) {
    spi.addr_p = addr;
    started = false;
}

void _CoreSPI::begin() {
    // Set the SPI0 pins to the Alt 0 function to enable SPI0 access on them
//    GPIO.selectFunction(CS0, FSEL_ALT0);
//    GPIO.selectFunction(CS1, FSEL_ALT0);
    GPIO.selectFunction(MISO, FSEL_ALT0);
    GPIO.selectFunction(MOSI, FSEL_ALT0);
    GPIO.selectFunction(SCK, FSEL_ALT0);

    map_peripheral(&spi);
    reg = (bcm2835_spi_registers *)spi.map;
    reg->cs_val = 0;
    sync_peripheral(&spi);
    reg->cs.clear = 0b11;
    reg->cs.cs = 0b11;

    sync_peripheral(&spi);

    setClockDivider(SPI_CLOCK_DIV256);
//    setChipSelectPolarity(SPI_CS0, LOW);
//    setChipSelectPolarity(SPI_CS1, LOW);
//    setChipSelectPolarity(SPI_CS2, LOW);
//    chipSelect(SPI_CS0);
    started = true;

}

void _CoreSPI::end(){
    // Set all the SPI0 pins back to input
//    GPIO.selectFunction(CS0, FSEL_INPT);
//    GPIO.selectFunction(CS1, FSEL_INPT);
    GPIO.selectFunction(MISO, FSEL_INPT);
    GPIO.selectFunction(MOSI, FSEL_INPT);
    GPIO.selectFunction(SCK, FSEL_INPT);
    started = false;
}


uint8_t _CoreSPI::transfer(uint8_t value) {
    reg->cs.clear = 0b11;
    reg->cs.ta = 1;
    sync_peripheral(&spi);
    while (reg->cs.txd == 0) {
        sync_peripheral(&spi);
        delayMicroseconds(10);
    }

    reg->fifo = value;
    sync_peripheral(&spi);

    while (reg->cs.done == 0) {
        sync_peripheral(&spi);
        delayMicroseconds(10);
    }

    sync_peripheral(&spi);
    uint32_t ret = reg->fifo;
    
    reg->cs.ta = 0;
    sync_peripheral(&spi);
    return ret;
}

void _CoreSPI::setBitOrder(uint8_t order) {
    // BCM2835_SPI_BIT_ORDER_MSBFIRST is the only one suported by SPI0
}

void _CoreSPI::setClockDivider(uint16_t divider) {
    reg->clk = divider;
    sync_peripheral(&spi);
}

void _CoreSPI::setDataMode(uint8_t mode) {
    reg->cs.cpol = (mode >> 1) & 0b1;
    reg->cs.cpha = mode & 0b1;
    sync_peripheral(&spi);
}

void _CoreSPI::transfer(unsigned char *tbuf, unsigned char *rbuf, uint32_t len) {
    reg->cs.clear = 0b11;
    reg->cs.ta = 1;
    sync_peripheral(&spi);

    for (uint32_t i = 0; i < len; i++) {
        while (reg->cs.txd == 0) {
            sync_peripheral(&spi);
            delayMicroseconds(10);
        }

        reg->fifo = tbuf[i];
        sync_peripheral(&spi);

        while (reg->cs.rxd == 0) {
            sync_peripheral(&spi);
            delayMicroseconds(10);
        }

        sync_peripheral(&spi);
        rbuf[i] = reg->fifo;
    }

    while (reg->cs.done == 0) {
        sync_peripheral(&spi);
        delayMicroseconds(10);
    }

    reg->cs.ta = 0;
    sync_peripheral(&spi);
    return;
}

//void _SPI::chipSelect(uint8_t cs) {
//    reg->cs.cs = cs;
//}
//
//void _SPI::setChipSelectPolarity(uint8_t cs, uint8_t active) {
//    switch (cs) {
//        case 0:
//            reg->cs.cspol0 = active ? HIGH : LOW;
//            break;
//        case 1:
//            reg->cs.cspol1 = active ? HIGH : LOW;
//            break;
//        case 2:
//            reg->cs.cspol2 = active ? HIGH : LOW;
//            break;
//    }
//    sync_peripheral(&spi);
//}

boolean _CoreSPI::isRunning() { return started; }

void _CoreSPI::saveState() {
    savedState = reg->cs_val;
    savedDiv = reg->clk;
}

void _CoreSPI::restoreState() {
    reg->cs_val = savedState;
    reg->clk = savedDiv;
    sync_peripheral(&spi);
}
