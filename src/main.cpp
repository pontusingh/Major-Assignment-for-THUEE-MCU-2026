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

static unsigned long lastOledRefresh = 0;
static const unsigned long OLED_REFRESH_MS = 500;
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
    sleepUpdate(state, now);
    feedingUpdate(state, now);
    actionUpdate(state, now);
    actionTryAutoTrigger(state, now);

    //输入处理
        //按钮
    if (buttonWasPressed()) {
        buttonClearFlag();
        if (!state.sleepMode) {
            Serial.print("["); Serial.print(now); Serial.println(" ms]  HeadTouch");
        }
        if (!sleepOnButtonPress(state, now)) {
            actionOnPat(state, now);
        }
    }

        //编码器
    if (!state.sleepMode && encoderCheckFullTurn()) {
        feedingTrigger(state, now);
        Serial.print("["); Serial.print(now); Serial.println(" ms]  Feed");
    }

    //串口命令解析
    parseSerialCommand(state, now);

    //OLED刷新
    if (now - lastOledRefresh >= OLED_REFRESH_MS) {
        lastOledRefresh = now;
        oledShowStatus(state, now);
    }

    //打印状态
    if (now - lastSerialPrint >= SERIAL_INTERVAL_MS) {
        lastSerialPrint = now;
        if (!state.sleepMode) {
            Serial.print("["); Serial.print(now); Serial.print(" ms]  EMOTION: ");
            Serial.print(state.moodValue);
            Serial.print(" ");
            if (state.isFull) {
                unsigned long elapsed = now - state.fullStateStartMs;
                unsigned long rem = (elapsed >= PetConfig::FULL_DURATION_MS)
                                   ? 0 : PetConfig::FULL_DURATION_MS - elapsed;
                Serial.print("FULL ");
                if (rem / 60000 < 10) Serial.print("0");
                Serial.print(rem / 60000);
                Serial.print(" Min ");
                if ((rem % 60000) / 1000 < 10) Serial.print("0");
                Serial.print((rem % 60000) / 1000);
                Serial.print(" S");
            } else {
                Serial.print("HUNGRY");
            }
            Serial.print(" ");
            switch (state.currentAction) {
                case ActionMode::NONE:    Serial.print("NULL"); break;
                case ActionMode::PLAY:    Serial.print("PLAY"); break;
                case ActionMode::IDLE:    Serial.print("IDLE"); break;
                case ActionMode::TIRED:   Serial.print("TIRE"); break;
                case ActionMode::FORWARD: Serial.print("FOWD"); break;
            }
            Serial.println();
        }
    }
}
