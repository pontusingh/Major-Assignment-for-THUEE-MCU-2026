#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>

void encoderInit();
int16_t encoderGetCount();
void encoderClearCount();
bool encoderCheckFullTurn();

#endif
