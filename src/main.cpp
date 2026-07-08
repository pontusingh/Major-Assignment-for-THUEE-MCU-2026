#include <Arduino.h>
#include "pin_config.h"
#include "servo.h"

void setup() {
    Serial.begin(115200);
    delay(500);

    servoInit(PIN_SERVO_LEFT, 0);
    servoInit(PIN_SERVO_RIGHT, 1);
    servoWrite(PIN_SERVO_LEFT, 90);
    servoWrite(PIN_SERVO_RIGHT, 90);
    delay(500);

    Serial.println("Dual servo test: 3 swings");
}

void loop() {
    for (int i = 1; i <= 3; i++) {
        servoWrite(PIN_SERVO_LEFT, 135);
        servoWrite(PIN_SERVO_RIGHT, 135);
        delay(300);

        servoWrite(PIN_SERVO_LEFT, 90);
        servoWrite(PIN_SERVO_RIGHT, 90);
        delay(300);

        servoWrite(PIN_SERVO_LEFT, 45);
        servoWrite(PIN_SERVO_RIGHT, 45);
        delay(300);

        servoWrite(PIN_SERVO_LEFT, 90);
        servoWrite(PIN_SERVO_RIGHT, 90);
        delay(300);

        Serial.print("Swing ");
        Serial.println(i);
    }

    Serial.println("Test done");
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
