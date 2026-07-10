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

enum class ActionMode : uint8_t {//动作枚举
    NONE    = 0,
    PLAY    = 1,
    IDLE    = 2,
    TIRED   = 3,
    FORWARD = 4
};

struct PetState {//核心结构体
    // Mood system
    int moodValue = PetConfig::MOOD_DEFAULT;
    unsigned long lastMoodDecayMs = 0;

    // Feeding system
    bool isFull = false;
    unsigned long fullStateStartMs = 0;
    unsigned long fullPauseElapsedMs = 0;

    // Motion / action system
    ActionMode currentAction = ActionMode::NONE;
    unsigned long actionStartMs = 0;
    unsigned long actionDurationMs = PetConfig::ACTION_DURATION_MS;

    // Interaction
    unsigned long lastPatMs = -PetConfig::PAT_COOLDOWN_MS;

    // Sleep mode
    bool sleepMode = false;
    unsigned long dimSinceMs = 0;

    // 选修部分: deep-sleep & Flash persistence
    bool wasSleeping = false;
    unsigned long lastInteractMs = 0;
    uint32_t patCount = 0;
    uint32_t feedCount = 0;
    uint32_t playCount = 0;
};

extern PetState state;

void resetState();
void saveToFlash();
void loadFromFlash();
