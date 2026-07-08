// pet_state.cpp
#include "pet_state.h"

PetState g_petState;

void PetState::resetState() {
    mood = static_cast<uint8_t>(MoodLevel::NEUTRAL);  // 初始值 5
    feeding = FeedingState::HUNGRY;
    action = ActionState::ACTION_NONE;
    sleep = SleepState::AWAKE;

    lastMoodDecayTime = millis();
    feedingStartTime = 0;
    actionStartTime = 0;
    lastActivityTime = millis();
    dimLightStartTime = 0;
    lastPatMs = 0;
    lastLoopTimeMs = millis();

    // Phase 4 字段暂时置零
    magic = 0;
    isFromDeepSleep = false;
    wifiConnected = false;
    lastNetworkPing = 0;
    memset(ssid, 0, sizeof(ssid));
}

// ---- Phase 4 之前这些函数都是空桩 ----
void PetState::saveToFlash() {
    // Phase 4 实现：保存到 Preferences (Flash) 和 RTC内存
}

void PetState::loadFromFlash() {
    // Phase 4 实现：从 Preferences/RTC 加载
}