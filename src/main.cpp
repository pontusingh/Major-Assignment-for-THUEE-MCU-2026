#include <Arduino.h>
#include "pin_config.h"
#include "oled.h"

void setup() {
    Serial.begin(115200);
    delay(500);

    oledInit();
    oledShowTest();

    Serial.println("OLED test done");
}

void loop() {
    vTaskDelay(pdMS_TO_TICKS(10000));
}
