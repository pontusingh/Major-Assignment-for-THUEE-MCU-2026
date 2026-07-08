#include <Wire.h>
#include <U8g2lib.h>
#include "pin_config.h"
#include "oled.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, PIN_OLED_SCL, PIN_OLED_SDA);

void oledInit() {
    Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);
    u8g2.begin();
    u8g2.setFont(u8g2_font_6x13_tf);
}

void oledShowTest() {
    u8g2.clearBuffer();
    u8g2.drawFrame(0, 0, 128, 64);
    u8g2.setCursor(20, 25);
    u8g2.print("OLED Test");
    u8g2.setCursor(20, 45);
    u8g2.print("Hello World");
    u8g2.sendBuffer();
}
