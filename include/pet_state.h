#pragma once
#include <Arduino.h>

namespace PetConfig {
    constexpr int MOOD_MIN = 1;
    constexpr int MOOD_MAX = 10;
    constexpr int MOOD_DEFAULT = 5;

    constexpr unsigned long MOOD_DECAY_INTERVAL_MS   = 30000;
    constexpr unsigned long FULL_DURATION_MS         = 120000;
    constexpr unsigned long PAT_COOLDOWN_MS          = 10000;
    constexpr unsigned long SLEEP_DIM_THRESHOLD_MS   = 5000;
    constexpr unsigned long ACTION_DURATION_MS       = 5000;
    constexpr unsigned long DEEP_SLEEP_INACTIVITY_MS = 30000;
}

inline int getMoodMax(uint32_t patCount) {
    if (patCount >= 16) return 10;
    if (patCount >= 6)  return 9;
    return 7;
}

inline unsigned long getPatCooldown(uint32_t patCount) {
    if (patCount >= 16) return 5000;
    if (patCount >= 6)  return 7000;
    return 10000;
}

inline int getAutoActionProb(uint32_t patCount) {
    if (patCount >= 16) return 80;
    if (patCount >= 6)  return 50;
    return 33;
}

enum class ActionMode : uint8_t {
    NONE    = 0,
    PLAY    = 1,
    IDLE    = 2,
    TIRED   = 3,
    FORWARD = 4
};

struct PetState {
    int moodValue = PetConfig::MOOD_DEFAULT;
    unsigned long lastMoodDecayMs = 0;

    bool isFull = false;
    unsigned long fullStateStartMs = 0;
    unsigned long fullPauseElapsedMs = 0;

    ActionMode currentAction = ActionMode::NONE;
    unsigned long actionStartMs = 0;
    unsigned long actionDurationMs = PetConfig::ACTION_DURATION_MS;

    unsigned long lastPatMs = -PetConfig::PAT_COOLDOWN_MS;

    bool sleepMode = false;
    unsigned long dimSinceMs = 0;

    unsigned long lastInteractMs = 0;
    uint32_t patCount = 0;
    uint32_t feedCount = 0;
};

extern PetState state;

void resetState();
