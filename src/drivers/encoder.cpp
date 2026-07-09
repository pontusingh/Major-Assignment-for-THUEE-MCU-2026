/*
 * 经验记录:
 *
 * PCNT 硬件正交解码: 双通道配置实现 4 倍频.
 *   ch0: A 相脉冲, B 相方向控制
 *   ch1: B 相脉冲, A 相方向控制
 * 硬件滤波器 (300 APB 周期 ~3.75µs) 消除机械回弹.
 *
 * 编码器参数 (实测标定):
 *   4x 正交解码模式 → 每 detent ±4 counts
 *   本编码器: 20 detents/转 → 80 counts/转
 *   圈数 = count / 80.0
 *
 * 方向判断: delta = currentCount - lastCount
 *   delta > 0 → CW, delta < 0 → CCW
 */
#include "driver/pcnt.h"
#include "pin_config.h"
#include "encoder.h"

static const pcnt_unit_t PCNT_UNIT = PCNT_UNIT_0;
static int16_t _prevCount = 0;
static unsigned long _lastChangeMs = 0;
static int16_t _contAccum = 0;

void encoderInit() {
    pinMode(PIN_ENCODER_A, INPUT_PULLUP);
    pinMode(PIN_ENCODER_B, INPUT_PULLUP);

    pcnt_config_t chA = {
        .pulse_gpio_num = PIN_ENCODER_A,
        .ctrl_gpio_num = PIN_ENCODER_B,
        .lctrl_mode = PCNT_MODE_REVERSE,
        .hctrl_mode = PCNT_MODE_KEEP,
        .pos_mode = PCNT_COUNT_INC,
        .neg_mode = PCNT_COUNT_DEC,
        .counter_h_lim = 32767,
        .counter_l_lim = -32768,
        .unit = PCNT_UNIT,
        .channel = PCNT_CHANNEL_0
    };
    ESP_ERROR_CHECK(pcnt_unit_config(&chA));

    pcnt_config_t chB = {
        .pulse_gpio_num = PIN_ENCODER_B,
        .ctrl_gpio_num = PIN_ENCODER_A,
        .lctrl_mode = PCNT_MODE_KEEP,
        .hctrl_mode = PCNT_MODE_REVERSE,
        .pos_mode = PCNT_COUNT_INC,
        .neg_mode = PCNT_COUNT_DEC,
        .counter_h_lim = 32767,
        .counter_l_lim = -32768,
        .unit = PCNT_UNIT,
        .channel = PCNT_CHANNEL_1
    };
    ESP_ERROR_CHECK(pcnt_unit_config(&chB));

    ESP_ERROR_CHECK(pcnt_set_filter_value(PCNT_UNIT, 300));
    ESP_ERROR_CHECK(pcnt_filter_enable(PCNT_UNIT));

    ESP_ERROR_CHECK(pcnt_counter_clear(PCNT_UNIT));
    ESP_ERROR_CHECK(pcnt_counter_resume(PCNT_UNIT));
}

int16_t encoderGetCount() {
    int16_t count = 0;
    ESP_ERROR_CHECK(pcnt_get_counter_value(PCNT_UNIT, &count));
    return count;
}

void encoderClearCount() {
    ESP_ERROR_CHECK(pcnt_counter_clear(PCNT_UNIT));
}

bool encoderCheckFullTurn() {
    int16_t count = encoderGetCount();
    unsigned long now = millis();
    int16_t delta = count - _prevCount;

    if (delta != 0) {
        _lastChangeMs = now;
        _prevCount = count;
        if (delta < 0) {
            _contAccum += delta;
        } else {
            _contAccum = 0;
        }
    } else if (now - _lastChangeMs >= 1000) {
        _contAccum = 0;
    }

    if (_contAccum <= -80) {
        encoderClearCount();
        _contAccum = 0;
        _prevCount = 0;
        return true;
    }

    return false;
}
