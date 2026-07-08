#include "servo.h"

#define MAX_SERVOS 2
#define PWM_FREQ 50
#define PWM_RES 12

static struct { uint8_t pin; uint8_t ch; } _servos[MAX_SERVOS];
static uint8_t _count = 0;

void servoInit(uint8_t pin, uint8_t ledcChannel) {
    if (_count >= MAX_SERVOS) return;
    double freq = ledcSetup(ledcChannel, PWM_FREQ, PWM_RES);
    if (!freq) return;
    ledcAttachPin(pin, ledcChannel);
    _servos[_count].pin = pin;
    _servos[_count].ch = ledcChannel;
    _count++;
}

void servoWrite(uint8_t pin, int angle) {
    angle = constrain(angle, 0, 180);
    for (int i = 0; i < _count; i++) {
        if (_servos[i].pin == pin) {
            uint32_t duty = map(angle, 0, 180, 164, 574);
            ledcWrite(_servos[i].ch, duty);
            return;
        }
    }
}
