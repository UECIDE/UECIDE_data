#include <SPI.h>

_SPI::_SPI() {
    didStart = false;
}

void _SPI::begin() {
    if (!CoreSPI.isRunning()) {
        CoreSPI.begin();
        didStart = true;
    }
}

void _SPI::end(){
    if (didStart) {
        CoreSPI.end();
    }
}

uint8_t _SPI::transfer(uint8_t value) {
    return CoreSPI.transfer(value);
}

void _SPI::setBitOrder(uint8_t order) {
    CoreSPI.setBitOrder(order);
}

void _SPI::setClockDivider(uint16_t divider) {
    CoreSPI.setClockDivider(divider);
}

void _SPI::setDataMode(uint8_t mode) {
    CoreSPI.setDataMode(mode);
}

void _SPI::transfer(unsigned char *tbuf, unsigned char *rbuf, uint32_t len) {
    CoreSPI.transfer(tbuf, rbuf, len);
}

boolean _SPI::isRunning() { return CoreSPI.isRunning(); }

void _SPI::saveState() {
    CoreSPI.saveState();
}

void _SPI::restoreState() {
    CoreSPI.restoreState();
}
