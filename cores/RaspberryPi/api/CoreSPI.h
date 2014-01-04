#ifndef _CORESPI_H
#define _CORESPI_H

#include <Arduino.h>

#define BCM2835_SPI0_BASE (IOBASE + 0x204000)

static const uint8_t SPI_MODE0 = 0;
static const uint8_t SPI_MODE1 = 1;
static const uint8_t SPI_MODE2 = 2;
static const uint8_t SPI_MODE3 = 3;

//static const uint8_t SPI_CS0 = 0;
//static const uint8_t SPI_CS1 = 1;
//static const uint8_t SPI_CS2 = 2;
//static const uint8_t SPI_CS_NONE = 3;

static const uint16_t SPI_CLOCK_DIV65536 = 0;       ///< 65536 = 256us = 4kHz
static const uint16_t SPI_CLOCK_DIV32768 = 32768;   ///< 32768 = 126us = 8kHz
static const uint16_t SPI_CLOCK_DIV16384 = 16384;   ///< 16384 = 64us = 15.625kHz
static const uint16_t SPI_CLOCK_DIV8192  = 8192;    ///< 8192 = 32us = 31.25kHz
static const uint16_t SPI_CLOCK_DIV4096  = 4096;    ///< 4096 = 16us = 62.5kHz
static const uint16_t SPI_CLOCK_DIV2048  = 2048;    ///< 2048 = 8us = 125kHz
static const uint16_t SPI_CLOCK_DIV1024  = 1024;    ///< 1024 = 4us = 250kHz
static const uint16_t SPI_CLOCK_DIV512   = 512;     ///< 512 = 2us = 500kHz
static const uint16_t SPI_CLOCK_DIV256   = 256;     ///< 256 = 1us = 1MHz
static const uint16_t SPI_CLOCK_DIV128   = 128;     ///< 128 = 500ns = = 2MHz
static const uint16_t SPI_CLOCK_DIV64    = 64;      ///< 64 = 250ns = 4MHz
static const uint16_t SPI_CLOCK_DIV32    = 32;      ///< 32 = 125ns = 8MHz
static const uint16_t SPI_CLOCK_DIV16    = 16;      ///< 16 = 50ns = 20MHz
static const uint16_t SPI_CLOCK_DIV8     = 8;       ///< 8 = 25ns = 40MHz
static const uint16_t SPI_CLOCK_DIV4     = 4;       ///< 4 = 12.5ns 80MHz
static const uint16_t SPI_CLOCK_DIV2     = 2;       ///< 2 = 6.25ns = 160MHz
static const uint16_t SPI_CLOCK_DIV1     = 1;       ///< 0 = 256us = 4kHz

class _CoreSPI {
    private:
        volatile struct bcm2835_peripheral spi;
        volatile bcm2835_spi_registers *reg;
        boolean started;
        uint32_t savedState;
        uint32_t savedDiv;

    public:
        _CoreSPI(uint32_t);
        void begin();
        void end();
        boolean isRunning();
        void setBitOrder(uint8_t order);
        void setClockDivider(uint16_t divider);
        void setDataMode(uint8_t mode);
//        void chipSelect(uint8_t cs);
//        void setChipSelectPolarity(uint8_t cs, uint8_t active);
        uint8_t transfer(uint8_t value);
        void transfer(unsigned char *tbuf, unsigned char *rbuf, uint32_t len);
        void saveState();
        void restoreState();
};

extern _CoreSPI CoreSPI;

#endif
