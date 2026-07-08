/*
 * 经验记录:
 *
 * 消抖方式: 在 ISR 内部用 millis() + 200ms 窗口过滤.
 * 原因是 attachInterrupt 触发极快 (ns 级),
 * 若在 loop() 中消抖, 第一次触发后到下一次 loop 迭代
 * 之间可能已经积累多次 bounce, pull-up 电阻导致按键
 * 在按下瞬间产生多次上升/下降沿跳变.
 *
 * 按键电平: INPUT_PULLUP, 按下为 LOW, 故用 FALLING 边沿.
 * ISR: ARDUINO_ISR_ATTR 确保在 IRAM 中执行,
 *      避免 flash cache 未命中导致 crash.
 */
#include "pin_config.h"
#include "button.h"

static volatile bool pressFlag = false;
static volatile unsigned long lastPressMs = 0;
static const unsigned long DEBOUNCE_MS = 200;

void ARDUINO_ISR_ATTR onButtonPress() {
    unsigned long now = millis();
    if (now - lastPressMs > DEBOUNCE_MS) {
        pressFlag = true;
        lastPressMs = now;
    }
}

void buttonInit() {
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), onButtonPress, FALLING);
}

bool buttonWasPressed() {
    return pressFlag;
}

void buttonClearFlag() {
    pressFlag = false;
}
