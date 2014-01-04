#ifndef _CONSOLE_SERIAL_H
#define _CONSOLE_SERIAL_H

#include <Arduino.h>

class ConsoleSerial : public Stream {
    public:
        void begin();
        void begin(uint32_t);
        void write(uint8_t);
        int available();
        int read();
        int peek();
        void flush();    
};

extern ConsoleSerial Console;

#endif

