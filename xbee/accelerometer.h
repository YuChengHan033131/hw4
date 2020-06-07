#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include "mbed.h"
//Serial pc(USBTX, USBRX);
void accelerometer(float &x, float &y ,float &z);
void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len);
void FXOS8700CQ_writeRegs(uint8_t * data, int len);

#endif