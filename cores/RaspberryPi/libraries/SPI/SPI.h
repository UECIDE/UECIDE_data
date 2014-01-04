#ifndef _SPI_H
#define _SPI_H

#include <Arduino.h>

class _SPI {
    private:
        boolean didStart;
    public:
        void begin();
        void end();
        boolean isRunning();
        void setBitOrder(uint8_t order);
        void setClockDivider(uint16_t divider);
        void setDataMode(uint8_t mode);
        uint8_t transfer(uint8_t value);
        void transfer(unsigned char *tbuf, unsigned char *rbuf, uint32_t len);
        void saveState();
        void restoreState();
};

extern _SPI SPI;

#endif
