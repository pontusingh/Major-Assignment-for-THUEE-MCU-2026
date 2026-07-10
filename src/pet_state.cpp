#include "pet_state.h"

// ---- RTC Memory 定义 (单一编译单元) ----
RTC_DATA_ATTR uint32_t      bootMs       = 0;
RTC_DATA_ATTR unsigned long lastInteract = 0;
RTC_DATA_ATTR bool           wasSleeping  = false;

PetState state;//全局实例

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
    state.playCount = 0;

    // 重置 RTC 变量 (仅首次上电 / 冷启动)
    bootMs = 0;
    lastInteract = 0;
    wasSleeping = false;
}

void saveToFlash() {
    // Phase 2
}

void loadFromFlash() {
    // Phase 2
}
