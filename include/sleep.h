#ifndef SLEEP_H
#define SLEEP_H

#include "pet_state.h"

void sleepUpdate(PetState& s, unsigned long nowMs);
bool sleepOnButtonPress(PetState& s, unsigned long nowMs);
void sleepWakeUp(PetState& s, unsigned long nowMs);

#endif
