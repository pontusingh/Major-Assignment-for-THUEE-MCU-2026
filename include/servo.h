#ifndef SERVO_H
#define SERVO_H

#include <Arduino.h>

void servoInit(uint8_t pin, uint8_t ledcChannel = 0);
void servoWrite(uint8_t pin, int angle);

#endif
