// pet_state.h
#ifndef PET_STATE_H
#define PET_STATE_H

#include <Arduino.h>

// ========== 来自规格说明的命名常量 ==========
// 情绪
const unsigned long MOOD_DECAY_INTERVAL_MS = 30000;  // 30秒衰减一次
const int MOOD_MIN = 1;
const int MOOD_MAX = 10;

// 交互
const unsigned long FEED_FULL_DURATION_MS = 120000;  // 2分钟饱足
const unsigned long PAT_COOLDOWN_MS = 10000;          // 抚摸冷却10秒

// 睡眠
const unsigned long SLEEP_DIM_LIGHT_DELAY_MS = 5000; // 5秒弱光触发睡眠
const unsigned long SLEEP_INACTIVITY_TIMEOUT_MS = 30000; // 30秒无活动（可选）

// 动作
const unsigned long ACTION_DURATION_MS = 5000;       // 每个动作默认持续5秒（可按需调整）

// ========== 枚举类型 ==========
enum class FeedingState : uint8_t {
    HUNGRY,
    FED
};

enum class ActionState : uint8_t {
    ACTION_NONE,
    PLAY,
    IDLE,
    TIRED,
    FORWARD
};

enum class SleepState : uint8_t {
    AWAKE,
    ASLEEP
};

// 仅保留 NEUTRAL=5 作为阈值比较（主状态改用 uint8_t mood）
enum class MoodLevel : uint8_t {
    NEUTRAL = 5
};

// ========== 核心状态结构体 ==========
struct PetState {
    // ---- 主状态 ----
    uint8_t mood;                 // 1–10 连续值
    FeedingState feeding;         // 饱足/饥饿
    ActionState action;           // 当前动作
    SleepState sleep;             // 清醒/睡眠

    // ---- 时间戳（用于计时逻辑） ----
    unsigned long lastMoodDecayTime;      // 上次情绪衰减时间
    unsigned long feedingStartTime;       // 进入饱足状态的时间
    unsigned long actionStartTime;        // 当前动作开始时间
    unsigned long lastActivityTime;       // 最后一次用户交互时间
    unsigned long dimLightStartTime;      // 弱光条件开始持续的时间
    unsigned long lastPatMs;              // 上次按键时间（每次按键均更新，用于冷却）
    unsigned long lastLoopTimeMs;         // 上次 loop() 时间戳（睡眠倒计时补偿）

    // ===== 阶段4/5预留字段（惰性占位，暂不使用） =====
    // Phase 4: 深度睡眠恢复
    uint32_t magic;                       // 校验魔数，验证RTC/Flash数据有效性
    bool isFromDeepSleep;                 // 是否从深度睡眠中恢复

    // Phase 5: 网络相关
    bool wifiConnected;                   // WiFi连接状态
    uint32_t lastNetworkPing;             // 上次网络心跳时间
    char ssid[32];                        // 保存的SSID（预留）
    // ============================================

    // ---- 方法声明 ----
    void resetState();                    // 重置为初始状态（Phase 2实现）
    void saveToFlash();                  // 保存到Flash/RTC（Phase 4实现）
    void loadFromFlash();                // 从Flash/RTC加载（Phase 4实现）
};

// ========== 全局实例声明（extern，在main.cpp中定义） ==========
extern PetState g_petState;

#endif // PET_STATE_H