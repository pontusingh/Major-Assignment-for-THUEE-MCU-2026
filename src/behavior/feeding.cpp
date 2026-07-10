#include "feeding.h"

void feedingUpdate(PetState& s, unsigned long nowMs) {
    if (s.isFull) {
        if (!s.sleepMode && nowMs - s.fullStateStartMs >= PetConfig::FULL_DURATION_MS) {
            s.isFull = false;
        }
    }
}

void feedingTrigger(PetState& s, unsigned long nowMs) {
    if (!s.sleepMode) {
        s.isFull = true;
        s.fullStateStartMs = nowMs;
        s.feedCount++;
        savePetMem();
    }
}
