#include <Wire.h>
#include <U8g2lib.h>
#include "pin_config.h"
#include "oled.h"

/*
 * 经验记录:
 *
 * 第一次上电 OLED 无显示 + 串口无输出,
 * 原因是 oledInit() 里调用了 Wire.begin(SDA, SCL)
 * 但 U8g2 构造器传了引脚 (U8G2_R0, U8X8_PIN_NONE, SCL, SDA),
 * u8g2.begin() 内部会再次调用 Wire.begin(),
 * 重复初始化导致 ESP32-S3 I2C 总线锁死.
 *
 * 两种正确做法(选一即可, 不要同时用):
 *   1. Lab4 模式: 构造器传引脚, 不调 Wire.begin()
 *   2. Lab3 模式: 构造器不传引脚, 单独调 Wire.begin(SDA, SCL)
 */
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, PIN_OLED_SCL, PIN_OLED_SDA);

void oledInit() {
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
