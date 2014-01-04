#include <Arduino.h>
#include <fcntl.h>

int mem_fd = -1;

int map_peripheral(volatile struct bcm2835_peripheral *p)
{
    if (mem_fd == -1) {
        mem_fd = open("/dev/mem", O_RDWR|O_SYNC);
    }
    if (mem_fd < 0) {
        printf("Failed to open /dev/mem. Are you running as root?\n");
        exit(10);
    }

    p->map = mmap(
        NULL,
        BLOCK_SIZE,
        PROT_READ|PROT_WRITE,
        MAP_SHARED,
        mem_fd, 
        p->addr_p 
    );

    if (p->map == MAP_FAILED) {
        printf("Failed to map peripheral memory. Aborting.\n");
        exit(10);
    }

    p->addr = (volatile unsigned int *)p->map;
    return 0;
}

void sync_peripheral(volatile struct bcm2835_peripheral *p) {
    msync(p->map, BLOCK_SIZE, MS_SYNC);
}
