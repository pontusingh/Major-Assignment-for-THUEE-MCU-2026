#include <Arduino.h>
#include "action.h"
#include "servo.h"
#include "pin_config.h"

static void _applyActionAngles(PetState& s, unsigned long elapsedMs) {
    float t = elapsedMs / 1000.0f;
    int left, right;

    switch (s.currentAction) {
        case ActionMode::NONE:
            left = 90; right = 90;
            break;
        case ActionMode::PLAY:
            left = 90 + (int)(45.0f * sinf(3.0f * PI * t));
            right = left;
            break;
        case ActionMode::IDLE:
            left = 60 + (int)(30.0f * cosf(1.5f * PI * t));
            right = left;
            break;
        case ActionMode::TIRED:
            left =130 + (int)(10.0f * cosf(4.0f / 3.0f * PI * t));
            right = 90;
            break;
        case ActionMode::FORWARD:
            left  = 125 - (int)(35.0f * cosf(1.5f * PI * t));
            right = 55 + (int)(35.0f * cosf(1.5f * PI * t));
            break;
    }

    servoWrite(PIN_SERVO_LEFT, left);
    servoWrite(PIN_SERVO_RIGHT, right);
}

void actionTrigger(PetState& s, ActionMode mode, unsigned long nowMs) {
    s.currentAction = mode;
    s.actionStartMs = nowMs;

    switch (mode) {
        case ActionMode::PLAY:    s.actionDurationMs = 2000; break;
        case ActionMode::IDLE:    s.actionDurationMs = 4000; break;
        case ActionMode::TIRED:   s.actionDurationMs = 3000; break;
        case ActionMode::FORWARD: s.actionDurationMs = 4000; break;
        default:                  s.actionDurationMs = PetConfig::ACTION_DURATION_MS; break;
    }

    _applyActionAngles(s, 0);
}

void actionUpdate(PetState& s, unsigned long nowMs) {
    if (s.currentAction == ActionMode::NONE) return;

    unsigned long elapsed = nowMs - s.actionStartMs;
    if (elapsed >= s.actionDurationMs) {
        actionTrigger(s, ActionMode::NONE, nowMs);
        return;
    }

    _applyActionAngles(s, elapsed);
}

void actionTryAutoTrigger(PetState& s, unsigned long nowMs) {
    if (s.currentAction != ActionMode::NONE) return;

    static unsigned long lastCheckMs = 0;
    if (nowMs - lastCheckMs < 1000) return;
    lastCheckMs = nowMs;

    if (random(1000) >= 33) return;

    ActionMode mode = (s.moodValue >= PetConfig::MOOD_DEFAULT)
                      ? ActionMode::IDLE : ActionMode::TIRED;
    actionTrigger(s, mode, nowMs);
}

void actionOnPat(PetState& s, unsigned long nowMs) {
    // 空桩——按钮接入后实现:
    // moodApplyPat(s, nowMs);
    // actionTrigger(s, ActionMode::PLAY, nowMs);
}
