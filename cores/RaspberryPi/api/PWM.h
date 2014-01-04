#ifndef _PWM_H
#define _PWM_H

#include <Arduino.h>

/* Due to the incredibly bad implementation on the broadcom chip
 * (i.e., sneaze and the PWM module crashes), and the complete
 * lack of any documentation (quite frankly I'd be too embarassed
 * to tell people too much about it) I have decided to implement
 * a software-based PWM system for this croe.  Also it will allow
 * you to use more than 1 (yes, the Pi only has ONE PWM pin!!!).
 */

class _PWM {
    private:
        int duty[DIGITAL_PINS];
        int phase;
    public:
        _PWM();
        void begin();
        void process();
        void analogWrite(uint8_t, uint8_t);
        void release(uint8_t);
};

extern _PWM PWM;

#endif
