#include "sleep.h"

bool sleepOnButtonPress(PetState& s, unsigned long nowMs) {
    if (s.sleepMode) {
        sleepWakeUp(s, nowMs);
        return true;
    }
    return false;
}

void sleepWakeUp(PetState& s, unsigned long nowMs) {
    s.sleepMode = false;
    s.dimSinceMs = 0;
}
