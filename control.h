#ifndef CONTROL_H
#define CONTROL_H

#include "mbed.h"

void control_fsm(void);
void control_init(DigitalOut* l,DigitalOut* r, DigitalOut* m, AnalogIn* v, BufferedSerial* s );

extern bool volatile  gb_control_can_sleep;


#endif

