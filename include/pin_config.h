#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

// ========== 光敏电阻 ==========
#define PIN_PHOTORESISTOR    4   // ADC1, GPIO4

// ========== OLED (I2C) ==========
#define PIN_OLED_SDA         8
#define PIN_OLED_SCL         9

// ========== 舵机 (LEDC) ==========
#define PIN_SERVO_LEFT       5   // 左臂
#define PIN_SERVO_RIGHT      6   // 右臂

// ========== 按键 ==========
#define PIN_BUTTON           7   // 轻拍（attachInterrupt）

// ========== 编码器 ==========
#define PIN_ENCODER_A       15   // 支持 PCNT
#define PIN_ENCODER_B       16

// ========== UART0（保留，勿改动）==========
#define PIN_UART0_RX        43   // 已被 USB/烧录占用
#define PIN_UART0_TX        44
#define PIN_SERIAL_RX       44   // 别名：Serial(RX=44, TX=43)
#define PIN_SERIAL_TX       43

// ========== 预留外设 ==========
#define PIN_ULTRASONIC_TRIG 17
#define PIN_ULTRASONIC_ECHO 18
#define PIN_DHT11           21

#endif // PIN_CONFIG_H
