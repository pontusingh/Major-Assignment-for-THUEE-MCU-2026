#include "pet_state.h"
#include "power_manager.h"

PetState state;//全局实例

void resetState() {//重置
    state.moodValue = PetConfig::MOOD_DEFAULT;
    state.lastMoodDecayMs = absoluteMs();

    state.isFull = false;
    state.fullStateStartMs = 0;

    state.currentAction = ActionMode::NONE;
    state.actionStartMs = 0;
    state.actionDurationMs = PetConfig::ACTION_DURATION_MS;

    state.lastPatMs = 0;

    state.sleepMode = false;
    state.dimSinceMs = 0;

    state.lastInteractMs = absoluteMs();
    state.patCount = 0;
    state.feedCount = 0;
}
