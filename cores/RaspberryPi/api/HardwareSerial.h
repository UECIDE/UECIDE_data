#ifndef _HARDWARE_SERIAL_H
#define _HARDWARE_SERIAL_H

#include <Arduino.h>

class HardwareSerial : public Stream {
    private:
        char *_port;
        int _fd;

    public:
        HardwareSerial(const char *path);
        void begin();
        void begin(uint32_t);
        void write(uint8_t);
        int available();
        int read();
        int peek();
        void flush();    
};

extern HardwareSerial Serial;

#endif

