#ifndef OLED_H
#define OLED_H

#include <Arduino.h>

void oledInit();
void oledShowTest();
void oledShowButtonTest(int pressCount, int lightVal);
void oledShowEncoderTest(int16_t count, int16_t delta, float revs);

#endif
