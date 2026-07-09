#ifndef FEEDING_H
#define FEEDING_H

#include "pet_state.h"

void feedingUpdate(PetState& s, unsigned long nowMs);
void feedingTrigger(PetState& s, unsigned long nowMs);

#endif
