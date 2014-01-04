#include <Arduino.h>

#include <unistd.h>
#include <sys/time.h>

static uint32_t bootMillis;

uint32_t getAbsMillis() {
    struct timeval tv;
    struct timezone tz;
    tz.tz_minuteswest = 0;
    tz.tz_dsttime = 0;
    gettimeofday(&tv, &tz);
    uint32_t m = tv.tv_sec * 1000;
    m = m + (tv.tv_usec / 1000);
    return m;
}


void initClocks() {
    bootMillis = getAbsMillis();
}

void delay(uint32_t m) {
    usleep(m * 1000UL);
}

void delayMicroseconds(uint32_t m) {
    usleep(m);
}

uint32_t millis() {
    uint32_t m = getAbsMillis() - bootMillis;
    return m;
}

uint32_t micros() {
    return millis() * 1000;
}
