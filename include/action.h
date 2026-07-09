#ifndef ACTION_H
#define ACTION_H

#include "pet_state.h"
#include <Arduino.h>

void actionTrigger(PetState& s, ActionMode mode, unsigned long nowMs);
void actionUpdate(PetState& s, unsigned long nowMs);
void actionTryAutoTrigger(PetState& s, unsigned long nowMs);
void actionOnPat(PetState& s, unsigned long nowMs);

#endif
