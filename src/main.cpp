#include <Arduino.h>
#include "pin_config.h"
#include "pet_state.h"
#include "oled.h"
#include "photoresistor.h"
#include "button.h"
#include "encoder.h"
#include "servo.h"
#include "mood.h"
#include "feeding.h"
#include "action.h"
#include "sleep.h"
#include "commands.h"

PetState g_petState;

static unsigned long lastOledRefresh = 0;
static const unsigned long OLED_REFRESH_MS = 100;
static unsigned long lastSerialPrint = 0;
static const unsigned long SERIAL_INTERVAL_MS = 5000;

void setup() {
    Serial.begin(115200);
    delay(500);

    oledInit();
    photoInit();
    buttonInit();
    encoderInit();
    servoInit(PIN_SERVO_LEFT, 0);
    servoInit(PIN_SERVO_RIGHT, 1);
    servoWrite(PIN_SERVO_LEFT, 90);
    servoWrite(PIN_SERVO_RIGHT, 90);

    g_petState.resetState();

    Serial.println("=== Desktop Pet Started ===");
}

void loop() {
    unsigned long now = millis();
    g_petState.lastLoopTimeMs = now;

    sleepUpdate(g_petState, now);
    moodUpdate(g_petState, now);
    feedingUpdate(g_petState, now);
    actionUpdate(g_petState, now);
    actionTryAutoTrigger(g_petState, now);

    if (buttonWasPressed()) {
        buttonClearFlag();
        if (!sleepOnButtonPress(g_petState, now)) {
            actionOnPat(g_petState, now);
        }
    }

    if (encoderCheckFullTurn()) {
        feedingTrigger(g_petState, now);
    }

    parseSerialCommand(g_petState);

    if (now - lastOledRefresh >= OLED_REFRESH_MS) {
        lastOledRefresh = now;
        oledShowStatus(g_petState);
    }

    if (now - lastSerialPrint >= SERIAL_INTERVAL_MS) {
        lastSerialPrint = now;
        if (g_petState.sleep != SleepState::ASLEEP) {
            Serial.print("[");
            Serial.print(now);
            Serial.print("] Mood:");
            Serial.print(static_cast<int>(g_petState.mood));
            Serial.print(" Action:");
            Serial.print(static_cast<int>(g_petState.action));
            Serial.print(" Feed:");
            Serial.print(static_cast<int>(g_petState.feeding));
            Serial.print(" Sleep:");
            Serial.print(static_cast<int>(g_petState.sleep));
            Serial.println();
        }
    }
}
