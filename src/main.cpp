#include <Arduino.h>
#include "pin_config.h"
#include "oled.h"
#include "photoresistor.h"

void setup() {
    Serial.begin(115200);
    delay(500);

    oledInit();
    oledShowTest();

    photoInit();

    Serial.println("Photoresistor test start");
}

void loop() {
    int val = photoReadDigital();
    Serial.print(">digital_light:");
    Serial.println(val);
    delay(50);
}
