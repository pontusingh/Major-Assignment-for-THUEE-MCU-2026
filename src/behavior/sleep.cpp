#include "sleep.h"
#include "photoresistor.h"
#include "servo.h"
#include "pin_config.h"

void sleepUpdate(PetState& s, unsigned long nowMs) {
    if (s.sleepMode) return;

    if (photoReadDigital() == 1) {
        if (s.dimSinceMs == 0) {
            s.dimSinceMs = nowMs;
        } else if (nowMs - s.dimSinceMs >= PetConfig::SLEEP_DIM_THRESHOLD_MS) {
            s.sleepMode = true;
            s.currentAction = ActionMode::NONE;
            servoWrite(PIN_SERVO_LEFT, 80);
            servoWrite(PIN_SERVO_RIGHT, 80);
            Serial.println("Sleep Mode On");
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
    servoWrite(PIN_SERVO_LEFT, 90);
    servoWrite(PIN_SERVO_RIGHT, 90);
    Serial.println("Wake Up");
}
