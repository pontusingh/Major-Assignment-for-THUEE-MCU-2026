#include <Arduino.h>
#include "pin_config.h"
#include "oled.h"
#include "photoresistor.h"
#include "button.h"
#include "encoder.h"

static int pressCount = 0;
static int16_t lastEncCount = 0;
static unsigned long lastOledRefresh = 0;
static const unsigned long OLED_REFRESH_MS = 100;

void setup() {
    Serial.begin(115200);
    delay(500);

    oledInit();
    photoInit();
    buttonInit();
    encoderInit();

    Serial.println("Encoder test start");
}

void loop() {
    if (buttonWasPressed()) {
        pressCount++;
        Serial.print("Press #");
        Serial.println(pressCount);
        buttonClearFlag();
    }

    int16_t currentCount = encoderGetCount();
    int16_t delta = currentCount - lastEncCount;
    if (delta != 0) {
        Serial.print("Count:");
        Serial.print(currentCount);
        Serial.print(" Delta:");
        Serial.print(delta);
        Serial.println(delta > 0 ? " CW" : " CCW");
        lastEncCount = currentCount;
    }

    Serial.print("raw:");
    Serial.println(currentCount);

    unsigned long now = millis();
    if (now - lastOledRefresh >= OLED_REFRESH_MS) {
        lastOledRefresh = now;
        float revs = currentCount / 80.0f;
        oledShowEncoderTest(currentCount, delta, revs);
    }

    delay(50);
}
