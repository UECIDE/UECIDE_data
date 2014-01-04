#include <Arduino.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/select.h>

ConsoleSerial Console;

void ConsoleSerial::begin() {
    print("\e[2J\e[1;1H");
    return;
}

void ConsoleSerial::begin(uint32_t baud) {
    print("\e[2J\e[1;1H");
    return;
}

void ConsoleSerial::write(uint8_t c) {
    putchar(c);
}

int ConsoleSerial::available() {
    fd_set rfds;
    struct timeval tv;
    int retval;

    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 10;

    retval = select(1, &rfds, NULL, NULL, &tv);
    if (retval > 0) {
        return 1;
    }
    return 0;
}

int ConsoleSerial::read() {
    if (!available()) {
        return -1;
    }
    int c = getchar();
    return c;
}

int ConsoleSerial::peek() {
    if (!available()) {
        return -1;
    }
    int c = getchar();
    ungetc(c, stdin);
    return c;
}

void ConsoleSerial::flush() {
}

