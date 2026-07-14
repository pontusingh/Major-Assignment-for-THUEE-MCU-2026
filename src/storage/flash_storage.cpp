#include "flash_storage.h"
#include <Preferences.h>
#include "power_manager.h"

static Preferences prefs;

void savePetMem() {
    prefs.begin("pet_mem", false);
    prefs.putUInt("pat",  state.patCount);
    prefs.putUInt("feed", state.feedCount);
    prefs.end();
}

void loadPetMemFromFlash() {
    prefs.begin("pet_mem", true);
    state.patCount  = prefs.getUInt("pat",  0);
    state.feedCount = prefs.getUInt("feed", 0);
    prefs.end();
}

void clearPetMem() {
    prefs.begin("pet_mem", false);
    prefs.clear();
    prefs.end();
    state.patCount = 0;
    state.feedCount = 0;
}

void saveToFlash() {
    prefs.begin("pet_cfg", false);
    prefs.putInt("mood",      state.moodValue);
    prefs.putBool("isFull",   state.isFull);
    prefs.putULong("fullStart", state.fullStateStartMs);
    prefs.putUChar("action",  static_cast<uint8_t>(state.currentAction));
    prefs.putBool("sleep",    state.sleepMode);
    prefs.putULong("lastDecay", state.lastMoodDecayMs);
    prefs.end();

    savePetMem();
}

void loadFromFlash() {
    //读取cfg命名空间
    prefs.begin("pet_cfg", true);
    state.moodValue     = prefs.getInt("mood", PetConfig::MOOD_DEFAULT);
    state.isFull        = prefs.getBool("isFull", false);
    unsigned long absFullStart = prefs.getULong("fullStart", 0);
    state.currentAction = static_cast<ActionMode>(
                              prefs.getUChar("action", 0));
    state.sleepMode     = prefs.getBool("sleep", false);
    state.lastMoodDecayMs = prefs.getULong("lastDecay", absoluteMs());
    prefs.end();

    //饱腹时间修复
    if (state.isFull && absFullStart != 0) {
        state.fullStateStartMs = absFullStart;
    } else {
        state.isFull = false;
        state.fullStateStartMs = absoluteMs();
    }

    state.lastPatMs       = 0;
    state.actionStartMs   = absoluteMs();

    //读取mem命名空间
    prefs.begin("pet_mem", true);
    state.patCount  = prefs.getUInt("pat",  0);
    state.feedCount = prefs.getUInt("feed", 0);
    prefs.end();
}

void incrementPatCount() {
    state.patCount++;
    savePetMem();
}

void incrementFeedCount() {
    state.feedCount++;
    savePetMem();
}
