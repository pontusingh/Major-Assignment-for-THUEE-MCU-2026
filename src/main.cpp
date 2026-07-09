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
//#include "sleep.h"
//#include "commands.h"

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

    resetState();
    randomSeed(esp_random());

    Serial.println("=== Desktop Pet Started ===");
}

void loop() {
    //确认时间
    unsigned long now = millis();

    //行为更新
    moodUpdate(state, now);
    //sleepUpdate(state, now);
    feedingUpdate(state, now);
    actionUpdate(state, now);
    actionTryAutoTrigger(state, now);

    //输入处理
        //按钮
    if (buttonWasPressed()) {
        buttonClearFlag();
        //sleepOnButtonPress / actionOnPat — TBD
    }

        //编码器
    if (encoderCheckFullTurn()) {
        feedingTrigger(state, now);
    }

    //串口命令解析
    //parseSerialCommand(state);

    //OLED刷新
    //if (now - lastOledRefresh >= OLED_REFRESH_MS) {
    //    lastOledRefresh = now;
    //    oledShowStatus(state);
    //}

    //打印状态
    if (now - lastSerialPrint >= SERIAL_INTERVAL_MS) {
        lastSerialPrint = now;
        if (!state.sleepMode) {
            Serial.print("[");
            Serial.print(now);
            Serial.print("] Mood:");
            Serial.print(state.moodValue);
            Serial.print(" Action:");
            Serial.print(static_cast<int>(state.currentAction));
            Serial.print(" Full:");
            Serial.print(state.isFull ? 1 : 0);
            if (state.isFull) {
                unsigned long elapsed = now - state.fullStateStartMs;
                if (elapsed >= PetConfig::FULL_DURATION_MS) {
                    Serial.print(" (0s remain)");
                } else {
                    unsigned long remain = PetConfig::FULL_DURATION_MS - elapsed;
                    Serial.print(" (");
                    Serial.print(remain / 1000);
                    Serial.print("s remain)");
                }
            }
            Serial.print(" Sleep:");
            Serial.print(state.sleepMode ? 1 : 0);
            Serial.print(" FeedCnt:");
            Serial.print(state.feedCount);
            Serial.println();
        }
    }
}
