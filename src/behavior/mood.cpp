#include "mood.h"

void moodUpdate(PetState& s, unsigned long nowMs) {
    if (s.sleep == SleepState::ASLEEP) return;
    if (s.feeding == FeedingState::FED) return;
    if (nowMs - s.lastMoodDecayTime >= MOOD_DECAY_INTERVAL_MS) {
        if (s.mood > MOOD_MIN) {
            s.mood--;
        }
        s.lastMoodDecayTime = nowMs;
    }
}
