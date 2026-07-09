#include "commands.h"
#include "action.h"
#include "sleep.h"
#include "servo.h"
#include "pin_config.h"

void parseSerialCommand(PetState& s, unsigned long nowMs) {
    if (!Serial.available()) return;

    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) return;

    int spaceIdx = line.indexOf(' ');
    String cmd = (spaceIdx == -1) ? line : line.substring(0, spaceIdx);
    String arg = (spaceIdx == -1) ? "" : line.substring(spaceIdx + 1);

    if (cmd == "setemo") {
        if (!s.sleepMode) {
            int val = constrain(arg.toInt(), PetConfig::MOOD_MIN, PetConfig::MOOD_MAX);
            s.moodValue = val;
            s.lastMoodDecayMs = nowMs;
        }
    } else if (cmd == "setful") {
        if (!s.sleepMode) {
            arg.toLowerCase();
            if (arg == "true") {
                s.isFull = true;
                s.fullStateStartMs = nowMs;
                s.fullPauseElapsedMs = 0;
            } else if (arg == "false") {
                s.isFull = false;
                s.fullPauseElapsedMs = 0;
            }
        }
    } else if (cmd == "setmot") {
        if (!s.sleepMode) {
            int val = arg.toInt();
            if (val >= 1 && val <= 4) {
                actionTrigger(s, (ActionMode)val, nowMs);
            }
        }
    } else if (cmd == "setslp") {
        arg.toLowerCase();
        if (arg == "true") {
            if (!s.sleepMode) {
                s.sleepMode = true;
                s.dimSinceMs = 0;
                s.currentAction = ActionMode::NONE;
                if (s.isFull) s.fullPauseElapsedMs = nowMs - s.fullStateStartMs;
                servoWrite(PIN_SERVO_LEFT, 80);
                servoWrite(PIN_SERVO_RIGHT, 80);
                Serial.println("Sleep Mode On");
            }
        } else if (arg == "false") {
            sleepWakeUp(s, nowMs);
        }
    } else {
        Serial.print("["); Serial.print(nowMs); Serial.print(" ms]  Unknown command: ");
        Serial.println(cmd);
    }
}
