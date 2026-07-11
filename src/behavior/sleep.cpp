#include "sleep.h"
#include "photoresistor.h"
#include "servo.h"
#include "pin_config.h"
#include "power_manager.h"

void sleepUpdate(PetState& s, unsigned long nowMs) {
    if (s.sleepMode) return;

    if (photoReadDigital() == 1) {
        if (s.dimSinceMs == 0) {
            s.dimSinceMs = nowMs;
        } else if (nowMs - s.dimSinceMs >= PetConfig::SLEEP_DIM_THRESHOLD_MS) {
            s.sleepMode = true;
            wasSleeping = true; // 标记: 唤醒后区分为睡眠唤醒
            if (s.isFull) s.fullPauseElapsedMs = nowMs - s.fullStateStartMs;
            s.currentAction = ActionMode::NONE;
            servoWrite(PIN_SERVO_LEFT, 10);
            servoWrite(PIN_SERVO_RIGHT, 10);
            Serial.print("["); Serial.print(nowMs); Serial.println(" ms]  Sleep Mode On");
        }
    } else {
        s.dimSinceMs = 0;
    }
}

bool sleepOnButtonPress(PetState& s, unsigned long nowMs) {
    if (s.sleepMode) {
        sleepWakeUp(s, nowMs);
        return true;
    }
    return false;
}

void sleepWakeUp(PetState& s, unsigned long nowMs) {
    s.sleepMode = false;
    s.dimSinceMs = 0;
    if (s.isFull) {
        s.fullStateStartMs = nowMs - s.fullPauseElapsedMs;
        s.fullPauseElapsedMs = 0;
    }
    Serial.print("["); Serial.print(nowMs); Serial.println(" ms]  Sleep Mode Off");
}
