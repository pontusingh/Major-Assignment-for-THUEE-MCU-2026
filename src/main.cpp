#include <Arduino.h>
#include "pin_config.h"
#include "oled.h"
#include "photoresistor.h"
#include "button.h"

static int pressCount = 0;
static unsigned long lastOledRefresh = 0;
static const unsigned long OLED_REFRESH_MS = 100;

void setup() {
    Serial.begin(115200);
    delay(500);

    oledInit();
    photoInit();
    buttonInit();

    Serial.println("Button test start");
}

void loop() {
    if (buttonWasPressed()) {
        pressCount++;
        Serial.print("Press #");
        Serial.println(pressCount);
        buttonClearFlag();
    }

    int lightVal = photoReadDigital();
    Serial.print(">digital_light:");
    Serial.println(lightVal);

    unsigned long now = millis();
    if (now - lastOledRefresh >= OLED_REFRESH_MS) {
        lastOledRefresh = now;
        oledShowButtonTest(pressCount, lightVal);
    }

    delay(50);
}
