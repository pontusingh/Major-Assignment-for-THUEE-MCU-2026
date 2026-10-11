# 智能桌面宠物（ESP32-S3 Smart Desktop Pet）

基于 ESP32-S3 的智能桌面宠物，通过 OLED 屏、双舵机、按键、编码器与光敏电阻，实现一套带有心情、饱食、动作与睡眠状态的电子宠物系统，并支持待机深睡、Flash 记忆与联网天气等扩展功能。

本项目为「thu电子系统专题设计与制作/硬设课」大作业，使用 PlatformIO + Arduino Core 2.x 开发。

---

## 1. 硬件组成

| 外设 | 引脚 | 接口类型 | 说明 |
|------|------|----------|------|
| 光敏电阻 | GPIO4 | ADC1 | 检测环境光强，用于睡眠判定 |
| OLED 屏幕（SDA / SCL） | GPIO8 / GPIO9 | I2C | 实时显示宠物状态 |
| 舵机（左臂 / 右臂） | GPIO5 / GPIO6 | LEDC（PWM） | 执行动作 |
| 按键（轻拍） | GPIO7 | GPIO 中断 | 摸头交互 / 唤醒 |
| 编码器（A / B） | GPIO15 / GPIO16 | GPIO 中断 | 喂食交互（整圈旋转触发） |
| 超声波（预留） | GPIO17 / GPIO18 | — | 预留扩展 |
| DHT11（预留） | GPIO21 | — | 预留扩展 |
| 串口 UART0 | GPIO43 / GPIO44 | UART | 烧录 / 调试，勿改动 |

开发板：`ESP32-S3-WROOM-1-N16R8`（16 MB Flash、8 MB PSRAM，OPI 接口）。

---

## 2. 核心功能

### 2.1 心情系统

- 心情值为 `1` ～ `10` 的整数，复位后为 `5`。
- 每 30 秒自然衰减 `1` 点，下限为 `1`；「吃饱」状态下不衰减。
- 心情影响自动动作的类型。

### 2.2 喂食系统

- 编码器连续顺时针旋转一整圈触发喂食，进入「吃饱（Full）」状态并重置 120 秒倒计时。
- 倒计时结束后自动回到「饥饿（Hungry）」状态。

### 2.3 动作系统

| 编号 | 动作 | 触发方式 |
|------|------|----------|
| 0 | 无动作（NULL） | 默认状态 |
| 1 | 玩耍（PLAY） | 按键轻拍 |
| 2 | 闲逛（IDLE） | 自动动作（心情 ≥ 5） |
| 3 | 疲惫（TIRE） | 自动动作（心情 < 5） |
| 4 | 前进（FOWD） | 串口命令 |

- 无动作时，每秒有 3.33% 概率触发自动动作。

### 2.4 交互系统

- **摸头**：点击按键，心情 +1（冷却 10 秒），立即中断当前动作并触发「玩耍」。
- **喂食**：编码器整圈顺时针旋转触发。
- OLED 三行显示：`EMOTION:x`、`HUNGRY` 或 `FULL xx Min yy s`、动作名。

### 2.5 睡眠系统

- 光敏电阻检测到环境持续暗 5 秒进入睡眠：清空 OLED、舵机抬至休眠位、停止响应编码器与串口命令（`setslp` 除外）、暂停心情衰减与饱食倒计时。
- 仅按键可唤醒，唤醒后回到中位，不增加心情、不触发动作。

---

## 3. 扩展功能

- **待机深睡**：30 秒无交互自动进入 Deep-sleep，BOOT 键唤醒，状态经 Flash 保存/恢复。
- **RTC 内存**：`bootMs`、`lastInteract`、`wasSleeping` 跨深睡保持。
- **Flash 记忆（Preferences）**：`pet_cfg`（状态）与 `pet_mem`（累计摸头/喂食次数）；摸头历史影响心情上限、冷却时间与自动动作概率。
- **联网天气**：WiFi 非阻塞连接，每 5 分钟拉取一次天气缓存显示，失败保留上次有效值。

---

## 4. 目录结构

```
Major-Assignment-Desktop-Pet/
├── platformio.ini          # PlatformIO 配置
├── include/                # 头文件（引脚、共享结构体、模块接口）
│   ├── pin_config.h        # 引脚宏定义
│   └── pet_state.h         # 共享状态结构体
├── src/
│   ├── main.cpp            # setup() / loop() 时序调度
│   ├── drivers/            # 硬件驱动层（不读写 pet_state）
│   │   ├── oled.cpp  servo.cpp  encoder.cpp
│   │   ├── photoresistor.cpp  button.cpp
│   ├── behavior/           # 业务逻辑层（读写 pet_state，调用 drivers）
│   │   ├── mood.cpp  feeding.cpp  action.cpp  sleep.cpp
│   ├── commands/           # 串口命令解析 parser.cpp
│   ├── power/              # 深睡/唤醒 power_manager.cpp
│   ├── storage/            # Flash 记忆 flash_storage.cpp
│   └── network/            # WiFi / 天气 wifi_manager.cpp  weather_client.cpp
└── test/                   # 单元测试
```

分层约定：`drivers/` 只管硬件；`behavior/` 读写 `pet_state` 并调用 `drivers`。

---

## 5. 构建与烧录

```bash
pio run                          # 编译
pio run --target upload          # 编译 + 烧录
pio run --target monitor         # 串口监视（115200）
pio test                         # 单元测试
```

依赖库：`olikraus/U8g2`、`bblanchon/ArduinoJson`。

---

## 6. 串口命令

| 命令 | 格式 | 说明 |
|------|------|------|
| 设置心情 | `setemo [argv]` | 直接设置心情值 |
| 设置饱食 | `setful true/false` | 设置饥饿/吃饱状态 |
| 设置动作 | `setmot [argv]` | 强制中断当前动作并触发指定动作 |
| 设置睡眠 | `setslp true/false` | 强制进入/退出睡眠 |

---

## 7. 调试输出

- 每条调试信息前缀为复位后时间：`[xxxx ms]`。
- 每 5 秒输出状态：`[ms] EMOTION:x HUNGRY/FULL ACTION | Head:x Feed:x`。
- 交互事件：`Head Touch`（摸头）、`Feed`（喂食）、`Sleep Mode On/Off`（睡眠）。
