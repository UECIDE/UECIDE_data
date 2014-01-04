#ifndef _REGISTERS_H
#define _REGISTERS_H

struct bcm2835_gpio_fsel {
    union {
        uint32_t value;
        struct {
            volatile unsigned fsel0:3;
            volatile unsigned fsel1:3;
            volatile unsigned fsel2:3;
            volatile unsigned fsel3:3;
            volatile unsigned fsel4:3;
            volatile unsigned fsel5:3;
            volatile unsigned fsel6:3;
            volatile unsigned fsel7:3;
            volatile unsigned fsel8:3;
            volatile unsigned fsel9:3;
            volatile unsigned reserved:2;
        } __attribute__((packed));
    } __attribute__((packed));
} __attribute__((packed));

struct bcm2835_gpio {
    volatile struct bcm2835_gpio_fsel gpfsel[6];
    volatile uint32_t reserved0;
    volatile uint32_t gpset[2];
    volatile uint32_t reserved1;
    volatile uint32_t gpclr[2];
    volatile uint32_t reserved2;
    volatile uint32_t gplev[2];
    volatile uint32_t reserved3;
    volatile uint32_t gpeds[2];
    volatile uint32_t reserved4;
    volatile uint32_t gpren[2];
    volatile uint32_t reserved5;
    volatile uint32_t gpfen[2];
    volatile uint32_t reserved6;
    volatile uint32_t gphen[2];
    volatile uint32_t reserved7;
    volatile uint32_t gplen[2];
    volatile uint32_t reserved8;
    volatile uint32_t gparen[2];
    volatile uint32_t reserved9;
    volatile uint32_t gpafen[2];
    volatile uint32_t reserved10;
    volatile uint32_t gppud;
    volatile uint32_t gppudclk[2];
    volatile uint32_t reserved11;
} __attribute__((packed));

struct bcm2835_spi_registers {
    union {
        uint32_t cs_val;
        struct {
            volatile unsigned cs:2;
            volatile unsigned cpha:1;
            volatile unsigned cpol:1;
            volatile unsigned clear:2;
            volatile unsigned cspol:1;
            volatile unsigned ta:1;
            volatile unsigned dmaen:1;
            volatile unsigned intd:1;
            volatile unsigned intr:1;
            volatile unsigned adcs:1;
            volatile unsigned ren:1;
            volatile unsigned len:1;
            volatile unsigned lmono:1;
            volatile unsigned te_en:1;
            volatile unsigned done:1;
            volatile unsigned rxd:1;
            volatile unsigned txd:1;
            volatile unsigned rxr:1;
            volatile unsigned rxf:1;
            volatile unsigned cspol0:1;
            volatile unsigned cspol1:1;
            volatile unsigned cspol2:1;
            volatile unsigned dma_len:1;
            volatile unsigned len_long:1;
            volatile unsigned reserved:6;
        } __attribute__((packed)) cs;
    } __attribute__((packed));
    volatile uint32_t fifo;
    volatile uint32_t clk;
    volatile uint32_t dlen;
    volatile uint32_t ltoh;
    union {
        volatile uint32_t dc_val;
        struct {
            volatile uint8_t tdreq;
            volatile uint8_t tpanic;
            volatile uint8_t rdreq;
            volatile uint8_t rpanic;
        } __attribute__((packed)) dc;
    } __attribute__((packed));
} __attribute__((packed));

struct bcm2835_pwm_registers {
    union {
        volatile uint32_t ctl_val;
        struct {
            volatile unsigned pwen1:1;
            volatile unsigned mode1:1;
            volatile unsigned rptl1:1;
            volatile unsigned sbit1:1;
            volatile unsigned pola1:1;
            volatile unsigned usef1:1;
            volatile unsigned clrf1:1;
            volatile unsigned msen1:1;
            volatile unsigned pwen2:1;
            volatile unsigned mode2:1;
            volatile unsigned rptl2:1;
            volatile unsigned sbit2:1;
            volatile unsigned pola2:1;
            volatile unsigned usef2:1;
            volatile unsigned reserved1:1;
            volatile unsigned msen2:1;
            volatile unsigned reserved:16;
        } __attribute__((packed)) ctl;
    } __attribute__((packed));
    union {
        volatile uint32_t sta_val;
        struct {
            volatile unsigned full1:1;
            volatile unsigned empt1:1;
            volatile unsigned werr1:1;
            volatile unsigned rerr1:1;
            volatile unsigned gapo1:1;
            volatile unsigned gapo2:1;
            volatile unsigned gapo3:1;
            volatile unsigned gapo4:1;
            volatile unsigned berr:1;
            volatile unsigned sta1:1;
            volatile unsigned sta2:1;
            volatile unsigned sta3:1;
            volatile unsigned sta4:1;
            volatile unsigned reserved:19;
        } __attribute__((packed)) sta;
    } __attribute__((packed));
    union {
        volatile uint32_t dmac_val;
        struct {
            volatile unsigned dreq:8;
            volatile unsigned panic:8;
            volatile unsigned reserved:15;
            volatile unsigned enab:1;
        } __attribute__((packed)) dmac;
    } __attribute__((packed));
    volatile uint32_t rng1;
    volatile uint32_t dat1;
    volatile uint32_t fifo;
    volatile uint32_t rng2;
    volatile uint32_t dat2;
} __attribute__((packed));

#endif
