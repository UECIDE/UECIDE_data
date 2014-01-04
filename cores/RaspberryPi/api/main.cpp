#include <Arduino.h>

extern void setup();
extern void loop();

void cleanupSystem() {
    for (int i = 0; i < DIGITAL_PINS; i++) {
        pinMode(i, INPUT);
    }
}

void initSystem() {
    GPIO.begin();
    initClocks();
    PWM.begin();
}

int main (){
    atexit(cleanupSystem);
    initSystem();
	setup();
	while(1){
		loop();
	}
	return (0);
}
