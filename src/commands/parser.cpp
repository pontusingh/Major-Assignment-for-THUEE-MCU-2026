#include "commands.h"
#include "action.h"
#include "sleep.h"
#include "servo.h"
#include "pin_config.h"

void parseSerialCommand(PetState& s, unsigned long nowMs) {
    if (!Serial.available()) return;
    Serial.setTimeout(20);
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) return;

    if (line.startsWith("setemo ")) {
        if (!s.sleepMode) {
            int val = constrain(line.substring(7).toInt(), PetConfig::MOOD_MIN, PetConfig::MOOD_MAX);
            s.moodValue = val;
            s.lastMoodDecayMs = nowMs;
        }
    } else if (line.startsWith("setful true")) {
        if (!s.sleepMode) {
            s.isFull = true;
            s.fullStateStartMs = nowMs;
            s.fullPauseElapsedMs = 0;
        }
    } else if (line.startsWith("setful false")) {
        if (!s.sleepMode) {
            s.isFull = false;
            s.fullPauseElapsedMs = 0;
        }
    } else if (line.startsWith("setmot ")) {
        if (!s.sleepMode) {
            int val = line.substring(7).toInt();
            if (val >= 0 && val <= 4) {
                actionTrigger(s, (ActionMode)val, nowMs);
            }
        }
    } else if (line.startsWith("setslp true")) {
        if (!s.sleepMode) {
            s.sleepMode = true;
            s.dimSinceMs = 0;
            s.currentAction = ActionMode::NONE;
            if (s.isFull) s.fullPauseElapsedMs = nowMs - s.fullStateStartMs;
            servoWrite(PIN_SERVO_LEFT, 80);
            servoWrite(PIN_SERVO_RIGHT, 80);
            Serial.println("Sleep Mode On");
        }
    } else if (line.startsWith("setslp false")) {
        sleepWakeUp(s, nowMs);
    } else {
        if (!s.sleepMode){
        Serial.print("["); Serial.print(nowMs); Serial.print(" ms]  Unknown command: ");
        Serial.println(line);
    }
    }
}
