#include <Arduino.h>
#include "pin_config.h"
#include "oled.h"
#include "photoresistor.h"
#include "button.h"
#include "encoder.h"
#include "servo.h"

static int pressCount = 0;
static int16_t lastEncCount = 0;
static unsigned long lastOledRefresh = 0;
static unsigned long lastSerialPrint = 0;
static const unsigned long OLED_REFRESH_MS = 100;
static const unsigned long SERIAL_INTERVAL_MS = 500;

static const char* dirStr(int16_t delta) {
    if (delta > 0) return "CW";
    if (delta < 0) return "CCW";
    return "--";
}

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

    Serial.println("=== Joint test start ===");
}

void loop() {
    if (buttonWasPressed()) {
        pressCount++;
        buttonClearFlag();
    }

    int16_t currentCount = encoderGetCount();
    int16_t delta = currentCount - lastEncCount;
    lastEncCount = currentCount;

    int lightVal = photoReadDigital();

    int leftAngle  = (pressCount * 30) % 181;
    int rightAngle = 180 - leftAngle;
    servoWrite(PIN_SERVO_LEFT, leftAngle);
    servoWrite(PIN_SERVO_RIGHT, rightAngle);

    float revs = currentCount / 80.0f;

    unsigned long now = millis();

    if (now - lastOledRefresh >= OLED_REFRESH_MS) {
        lastOledRefresh = now;
        oledShowJointTest(currentCount, dirStr(delta), revs,
                          pressCount, lightVal,
                          leftAngle, rightAngle);
    }

    if (now - lastSerialPrint >= SERIAL_INTERVAL_MS) {
        lastSerialPrint = now;
        Serial.print("=== Joint === Enc:");
        Serial.print(currentCount);
        Serial.print(" ");
        Serial.print(dirStr(delta));
        Serial.print(" ");
        Serial.print(revs, 1);
        Serial.print("r  Btn:");
        Serial.print(pressCount);
        Serial.print("  Light:");
        Serial.print(lightVal);
        Serial.print("  L:");
        Serial.print(leftAngle);
        Serial.print((char)176);
        Serial.print(" R:");
        Serial.print(rightAngle);
        Serial.print((char)176);
        Serial.println();
    }

    delay(50);
}
