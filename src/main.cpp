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

    // ---- 选修--启动检测 ----
    BootCause boot = detectBootCause();

    if (boot == BootCause::COLD_BOOT) {
        resetState();
        loadPetMemFromFlash();
        servoWrite(PIN_SERVO_LEFT, 90);
        servoWrite(PIN_SERVO_RIGHT, 90);
        Serial.println("=== Desktop Pet Started ===");
    } else {
        // deep-sleep 唤醒: 从 Flash 恢复 PetState
        loadFromFlash();

        if (boot == BootCause::SLEEP_WAKE) {
            // 睡眠唤醒: 保持 sleepMode, 不归中立位, 不触发动作
            state.sleepMode = true;
            servoWrite(PIN_SERVO_LEFT, 10);
            servoWrite(PIN_SERVO_RIGHT, 10);
            Serial.println("=== Woke from SLEEP ===");
        } else {
            // 待机唤醒: 恢复正常运行
            state.sleepMode = false;
            servoWrite(PIN_SERVO_LEFT, 90);
            servoWrite(PIN_SERVO_RIGHT, 90);
            Serial.println("=== Woke from STANDBY ===");
        }

        // 重置交互时间戳，给用户 30s 交互窗口
        state.lastInteractMs = millis();
        lastInteract = bootMs + millis();
    }

    randomSeed(esp_random());
}

void loop() {
    //确认时间
    unsigned long now = millis();

    //串口命令解析（最高优先级）
    parseSerialCommand(state, now);

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
            Serial.print(" | Head:");
            Serial.print(state.patCount);
            Serial.print(" Feed:");
            Serial.print(state.feedCount);
            Serial.println();
        }
    }
}
