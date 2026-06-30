#include <Arduino.h>

// GPIO 外设寄存器基地址
#define GPIO_BASE_ADDR        0x60004000
// GPIO 0-31 输出使能置位寄存器
#define GPIO_ENABLE_W1TS_ADDR ((volatile uint32_t*)(GPIO_BASE_ADDR + 0x24))
// GPIO 0-31 输出置位寄存器
#define GPIO_OUT_W1TS_ADDR    ((volatile uint32_t*)(GPIO_BASE_ADDR + 0x08))
// GPIO 0-31 输出清零寄存器
#define GPIO_OUT_W1TC_ADDR    ((volatile uint32_t*)(GPIO_BASE_ADDR + 0x0C))

#define LED_PIN 10
#define INTERVAL 1000

void setup()
{   
    // 设置为通用输出模式
    *GPIO_ENABLE_W1TS_ADDR = (1 << LED_PIN);
}

void loop()
{   
    // 输出高电平
    *GPIO_OUT_W1TS_ADDR = (1 << LED_PIN);
    // 延迟
    vTaskDelay(pdMS_TO_TICKS(INTERVAL));

    // 输出低电平
    *GPIO_OUT_W1TC_ADDR = (1 << LED_PIN);
    // 延迟
    vTaskDelay(pdMS_TO_TICKS(INTERVAL));
}