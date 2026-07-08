/*
 * 经验记录:
 *
 * 首次测试同时读取 analog + digital 发现两个信号都不正常:
 *   - digitalLight 始终为 0 (ADC 模式占用了引脚, 数字输入缓冲不工作)
 *   - analogLight 呈方波而非锯齿波 (每次 analogRead 前都要重新切换、
 *     未稳定就采样, 读数在高低值之间跳变)
 *
 * 教训: 不要在 ESP32 的同一个引脚上交替调用 analogRead() 和
 *       digitalRead() — GPIO 矩阵的切换会互相干扰.
 *
 * 实际硬件是数字光敏模块 (带比较器, 直接输出 0/1),
 * 只用 digitalRead(PIN_PHOTORESISTOR) 即可, 但保留 photoReadAnalog()
 * 接口以备后续项目可能使用模拟光敏电阻.
 */
#include "pin_config.h"
#include "photoresistor.h"

void photoInit() {
    pinMode(PIN_PHOTORESISTOR, INPUT);
}

int photoReadAnalog() {
    return analogRead(PIN_PHOTORESISTOR);
}

int photoReadDigital() {
    return digitalRead(PIN_PHOTORESISTOR);
}
