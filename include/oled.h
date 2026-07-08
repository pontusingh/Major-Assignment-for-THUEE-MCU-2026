#ifndef OLED_H
#define OLED_H

#include <Arduino.h>

void oledInit();
void oledShowTest();
void oledShowButtonTest(int pressCount, int lightVal);
void oledShowEncoderTest(int16_t count, int16_t delta, float revs);
void oledShowJointTest(int16_t encCount, const char* encDir, float revs,
                       int btnCount, int lightVal,
                       int servoLAngle, int servoRAngle);

#endif
