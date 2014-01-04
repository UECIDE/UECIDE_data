#include <Arduino.h>
#include <unistd.h>

_PWM PWM;
Thread PWMThread;

void CorePWMProcessor() {
    PWM.process();
}

_PWM::_PWM() {
    for (int i = 0; i < DIGITAL_PINS; i++) {
        duty[i] = -1;
    }
    phase = 0;
}

void _PWM::begin() {
    PWMThread.execute(CorePWMProcessor);
}

void _PWM::analogWrite(uint8_t pin, uint8_t val) {
    uint8_t pipin = raspberryPinNumber(pin);

    if (pipin == NOT_A_PIN) {
        return;
    }

    GPIO.pinMode(pin, OUTPUT);
    duty[pin] = val;
}

void _PWM::release(uint8_t pin) {
    uint8_t pipin = raspberryPinNumber(pin);

    if (pipin == NOT_A_PIN) {
        return;
    }

    if (duty[pin] == -1) {
        return;
    }

    duty[pin] = -1;
    GPIO.digitalWrite(pin, LOW);
}

void _PWM::process() {
    if (phase == 0) {
        for (int i = 0; i < DIGITAL_PINS; i++) {
            if (duty[i] != -1) {
                GPIO.digitalWrite(i, duty[i] == 0 ? LOW : HIGH);
            }
        }
    } else {
        for (int i = 0; i < DIGITAL_PINS; i++) {
            if (duty[i] <= phase) {
                GPIO.digitalWrite(i, LOW);
            }
        }
    }

    int d = 255 - phase;
    for (int i = 0; i < DIGITAL_PINS; i++) {
        if ((duty[i] > phase) && (duty[i] - phase) < d) {
            d = duty[i] - phase;
        }
    }
    delayMicroseconds(d * 10);
    phase += d;
    if (phase >= 255) {
        phase = 0;
    }
}

