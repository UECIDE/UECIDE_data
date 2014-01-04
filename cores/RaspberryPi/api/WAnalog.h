#ifndef _WANALOG_H
#define _WANALOG_H

extern void analogInit();
extern uint16_t analogRead(uint8_t pin);
extern void analogWrite(uint8_t pin, uint8_t val);

#endif
