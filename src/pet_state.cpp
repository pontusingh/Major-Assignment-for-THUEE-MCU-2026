#include "pet_state.h"
#include <Preferences.h>

// ---- RTC Memory 定义 (单一编译单元) ----
RTC_DATA_ATTR uint32_t      bootMs       = 0;
RTC_DATA_ATTR unsigned long lastInteract = 0;
RTC_DATA_ATTR bool           wasSleeping  = false;

PetState state;//全局实例
static Preferences prefs;

BootCause detectBootCause() {
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();

    if (cause == ESP_SLEEP_WAKEUP_GPIO) {
        // BOOT 键唤醒，根据 wasSleeping 区分待机/睡眠
        return wasSleeping ? BootCause::SLEEP_WAKE
                           : BootCause::STANDBY_WAKE;
    }

    // 首次上电: RTC 变量均为初始值 0
    if (bootMs == 0 && lastInteract == 0) {
        return BootCause::COLD_BOOT;
    }

    // 异常复位 (如复位按钮)，按冷启动处理
    return BootCause::COLD_BOOT;
}

void resetState() {//重置
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

    state.lastInteractMs = millis();
    state.patCount = 0;
    state.feedCount = 0;

    // 重置 RTC 变量 (仅首次上电 / 冷启动)
    bootMs = 0;
    lastInteract = 0;
    wasSleeping = false;
}

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
    // ---- pet_cfg: 进入 deep-sleep 前保存 ----
    prefs.begin("pet_cfg", false);
    prefs.putInt("mood",      state.moodValue);
    prefs.putBool("isFull",   state.isFull);
    prefs.putULong("fullStart", bootMs + state.fullStateStartMs); // 绝对时间
    prefs.putUChar("action",  static_cast<uint8_t>(state.currentAction));
    prefs.putBool("sleep",    state.sleepMode);
    prefs.end();

    // ---- pet_mem: 累计计数 ----
    savePetMem();
}

void loadFromFlash() {
    // ---- pet_cfg ----
    prefs.begin("pet_cfg", true);
    state.moodValue     = prefs.getInt("mood", PetConfig::MOOD_DEFAULT);
    state.isFull        = prefs.getBool("isFull", false);
    unsigned long absFullStart = prefs.getULong("fullStart", 0);
    state.currentAction = static_cast<ActionMode>(
                              prefs.getUChar("action", 0));
    state.sleepMode     = prefs.getBool("sleep", false);
    prefs.end();

    // 恢复 fullStateStartMs: absTime -> millis()-relative
    if (state.isFull && absFullStart > bootMs) {
        state.fullStateStartMs = absFullStart - bootMs;
    } else {
        state.isFull = false;  // 数据无效，放弃恢复
        state.fullStateStartMs = millis();
    }

    // 重置 millis()-relative 时间戳
    state.lastMoodDecayMs = millis();
    state.lastPatMs       = 0;
    state.actionStartMs   = millis();

    // ---- pet_mem ----
    prefs.begin("pet_mem", true);
    state.patCount  = prefs.getUInt("pat",  0);
    state.feedCount = prefs.getUInt("feed", 0);
    prefs.end();
}
