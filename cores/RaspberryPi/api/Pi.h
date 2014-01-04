#ifndef _PI_H
#define _PI_H

#include <errno.h>
#include <sys/mman.h>

//extern struct bcm2835_peripheral gpio;

#define IOBASE   0x20000000
#define PAGESIZE 4096
#define BLOCK_SIZE 4096

struct bcm2835_peripheral {
    unsigned long addr_p;
    void *map;
    volatile unsigned int *addr;
};

extern void sync_peripheral(volatile struct bcm2835_peripheral *p);
extern int map_peripheral(volatile struct bcm2835_peripheral *p);
extern void initPi();
extern void bcm2835_gpio_fsel(uint8_t pin, uint8_t mode);
extern uint32_t bcm2835_peri_read(volatile uint32_t* paddr);
extern uint32_t bcm2835_peri_read_nb(volatile uint32_t* paddr);
extern void bcm2835_peri_write(volatile uint32_t* paddr, uint32_t value);
extern void bcm2835_peri_write_nb(volatile uint32_t* paddr, uint32_t value);
extern void bcm2835_peri_set_bits(volatile uint32_t* paddr, uint32_t value, uint32_t mask);


#endif
