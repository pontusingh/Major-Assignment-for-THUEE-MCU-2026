#include "pet_state.h"

PetState state;

void resetState() {
    state.moodValue = PetConfig::MOOD_DEFAULT;
    state.lastMoodDecayMs = millis();

    state.isFull = false;
    state.fullStateStartMs = 0;

    state.currentAction = ActionMode::NONE;
    state.actionStartMs = 0;
    state.actionDurationMs = PetConfig::ACTION_DURATION_MS;

    state.lastPatMs = 0;

    state.sleepMode = false;
    state.dimSinceMs = 0;

    state.lastStatusPrintMs = 0;

    state.wasSleeping = false;
    state.lastInteractMs = millis();
    state.patCount = 0;
    state.feedCount = 0;
    state.playCount = 0;
}

void saveToFlash() {
    // Phase 4
}

void loadFromFlash() {
    // Phase 4
}
