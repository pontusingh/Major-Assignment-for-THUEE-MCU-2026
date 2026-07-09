#include "mood.h"

void moodUpdate(PetState& s, unsigned long nowMs) {
    if (s.sleepMode) return;
    if (s.isFull) return;
    if (nowMs - s.lastMoodDecayMs >= PetConfig::MOOD_DECAY_INTERVAL_MS) {
        if (s.moodValue > PetConfig::MOOD_MIN) {
            s.moodValue--;
        }
        s.lastMoodDecayMs = nowMs;
    }
}

void moodApplyPat(PetState& s, unsigned long nowMs) {
    if (nowMs - s.lastPatMs >= PetConfig::PAT_COOLDOWN_MS) {
        if (s.moodValue < PetConfig::MOOD_MAX) s.moodValue++;
    }
    s.lastPatMs = nowMs;
    s.lastMoodDecayMs = nowMs;
}
