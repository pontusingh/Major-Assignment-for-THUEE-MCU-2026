#include <Arduino.h>
#include "pin_config.h"
#include "servo.h"

void setup() {
    Serial.begin(115200);
    delay(500);

    servoInit(PIN_SERVO_LEFT);
    servoWrite(PIN_SERVO_LEFT, 90);
    delay(500);

    Serial.println("Servo test start");
}

void loop() {
    for (int angle = 0; angle <= 180; angle += 10) {
        Serial.print("Angle: ");
        Serial.println(angle);
        servoWrite(PIN_SERVO_LEFT, angle);
        delay(500);
    }
    for (int angle = 180; angle >= 0; angle -= 10) {
        Serial.print("Angle: ");
        Serial.println(angle);
        servoWrite(PIN_SERVO_LEFT, angle);
        delay(500);
    }
}
