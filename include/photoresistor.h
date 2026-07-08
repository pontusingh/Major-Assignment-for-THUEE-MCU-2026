#ifndef PHOTORESISTOR_H
#define PHOTORESISTOR_H

#include <Arduino.h>

void photoInit();
int photoReadAnalog();
int photoReadDigital();

#endif
