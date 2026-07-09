#include "feeding.h"

void feedingUpdate(PetState& s, unsigned long nowMs) {
    if (s.feeding == FeedingState::FED) {
        if (s.sleep == SleepState::ASLEEP) {
            s.feedingStartTime += (nowMs - s.lastLoopTimeMs);
        } else if (nowMs - s.feedingStartTime >= FEED_FULL_DURATION_MS) {
            s.feeding = FeedingState::HUNGRY;
        }
    }
}

void feedingTrigger(PetState& s, unsigned long nowMs) {
    if (s.sleep == SleepState::AWAKE) {
        s.feeding = FeedingState::FED;
        s.feedingStartTime = nowMs;
    }
}
