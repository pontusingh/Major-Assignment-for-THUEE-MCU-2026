#ifndef OLED_H
#define OLED_H

#include <Arduino.h>
#include "pet_state.h"

void oledInit();
void oledShowStatus(PetState& s, unsigned long nowMs);

#endif
