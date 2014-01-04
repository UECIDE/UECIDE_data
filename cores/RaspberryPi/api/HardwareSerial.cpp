#include <Arduino.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/select.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>

HardwareSerial Serial("/dev/ttyAMA0");

HardwareSerial::HardwareSerial(const char *dev) {
    _port = strdup(dev);
    _fd = -1;
}

void HardwareSerial::begin() {
    begin(9600);
}

void HardwareSerial::begin(uint32_t baud) {
    struct termios tc;
    int status;

    _fd = open(_port, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
    if (_fd == -1) {
        return;
    }

    fcntl(_fd, F_SETFL, O_RDWR);
    tcgetattr(_fd, &tc);
    cfmakeraw(&tc);
    cfsetspeed(&tc, baud);
    tc.c_cflag |= (CLOCAL | CREAD);
    tc.c_cflag &= ~PARENB;

    tc.c_cflag |= (CLOCAL | CREAD);
    tc.c_cflag &= ~PARENB;
    tc.c_cflag &= ~CSTOPB;
    tc.c_cflag &= ~CSIZE;
    tc.c_cflag |= CS8;
    tc.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tc.c_oflag &= ~OPOST;

    tcsetattr (_fd, TCSANOW, &tc);

    ioctl(_fd, TIOCMGET, &status);
    status |= TIOCM_DTR;
    status |= TIOCM_RTS;
    ioctl(_fd, TIOCMSET, &status);
}

void HardwareSerial::write(uint8_t c) {
    if (_fd == -1) {
        return;
    }
    ::write(_fd, &c, 1);
}

int HardwareSerial::available() {
    fd_set rfds;
    struct timeval tv;
    int retval;
    if (_fd == -1) {
        return -1;
    }

    FD_ZERO(&rfds);
    FD_SET(_fd, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 10;

    retval = select(_fd+1, &rfds, NULL, NULL, &tv);
    if (retval > 0) {
        return 1;
    }
    return 0;
}

int HardwareSerial::read() {
    uint8_t c;
    if (_fd == -1) {
        return -1;
    }
    if (!available()) {
        return -1;
    }
    ::read(_fd, &c, 1);
    return c;
}

int HardwareSerial::peek() {
    if (_fd == -1) {
        return -1;
    }
    if (!available()) {
        return -1;
    }
    FILE *fp = fdopen(_fd, "r");
    int c = fgetc(fp);
    ungetc(c, fp);
    return c;
}

void HardwareSerial::flush() {
}

