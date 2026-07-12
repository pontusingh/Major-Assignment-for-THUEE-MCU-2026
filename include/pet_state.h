#pragma once//头文件保护
#include <Arduino.h>

namespace PetConfig {//可调变量放入命名空间
    constexpr int MOOD_MIN = 1;
    constexpr int MOOD_MAX = 10;
    constexpr int MOOD_DEFAULT = 5;

    constexpr unsigned long MOOD_DECAY_INTERVAL_MS   = 30000;//情绪减一
    constexpr unsigned long FULL_DURATION_MS         = 120000;//饱食转饥饿
    constexpr unsigned long PAT_COOLDOWN_MS          = 10000;//按钮冷却
    constexpr unsigned long SLEEP_DIM_THRESHOLD_MS   = 5000;//低光照睡眠
    constexpr unsigned long ACTION_DURATION_MS       = 5000;//动作周期
    constexpr unsigned long DEEP_SLEEP_INACTIVITY_MS = 30000;//选修部分
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

struct PetState {//核心结构体
    // mood system
    int moodValue = PetConfig::MOOD_DEFAULT;
    unsigned long lastMoodDecayMs = 0;

    // feeding 
    bool isFull = false;
    unsigned long fullStateStartMs = 0;
    unsigned long fullPauseElapsedMs = 0;

    // action
    ActionMode currentAction = ActionMode::NONE;
    unsigned long actionStartMs = 0;
    unsigned long actionDurationMs = PetConfig::ACTION_DURATION_MS;

    //interaction
    unsigned long lastPatMs = -PetConfig::PAT_COOLDOWN_MS;

    // sleep
    bool sleepMode = false;
    unsigned long dimSinceMs = 0;

    //选修部分-flash
    unsigned long lastInteractMs = 0;
    uint32_t patCount = 0;
    uint32_t feedCount = 0;

    // WiFi
    bool wifiConnected = false;

    // Weather
    unsigned long lastFetchAttemptMs = 0;
    bool lastFetchOk = false;
    char cachedWeatherText[32] = "--";
};

extern PetState state;

void resetState();
