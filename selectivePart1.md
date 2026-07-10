# 一、运用的技术

## 1.1 Deep-sleep 待机
正常 Active 运行 (loop() 循环)，长时间无操作或低光照条件下触发 Deep-sleep。

### 唤醒与恢复：
- 唤醒后从 setup() 重新开始，通过 esp_sleep_get_wakeup_cause() 判断唤醒源
- 进入 Deep-sleep 前 saveToFlash() 保存完整状态，唤醒后 loadFromFlash() 恢复
- 待机唤醒 vs 睡眠唤醒：进入 Deep-sleep 前将标志写入 RTC_DATA_ATTR wasSleeping，唤醒后读取区分（待机唤醒可交互）

## 1.2 RTC Memory
RTC_DATA_ATTR 变量保持跨睡眠数据，核心三个：

| 变量 | 作用 |
|------|------|
| bootMs | 累计运行时间 |
| lastInteract | 上次交互时间戳（用于 30 秒待机判定） |
| wasSleeping | 入睡前模式标志（区分待机/睡眠唤醒） |

## 1.3 Preferences Flash 记忆
双命名空间：

| 命名空间 | 用途 | 写入时机 |
|----------|------|----------|
| pet_cfg | 心情、饱食、动作状态 | 进入 Deep-sleep 前（掉电保护） |
| pet_mem | 累计摸头/喂食/玩耍次数 | 交互发生时 |

### 记忆影响行为（自行设计具体规则）：
- 摸头次数影响心情上限、摸头冷却时间、自动动作触发概率
- 上电时一次性从 Flash 读到 RAM 缓存，避免运行时频繁读 Flash


# 二、运行效果

## 正常模式
OLED 三行显示：EMOTION:x / HUNGRY或FULL xm xs / 动作名

- 每 30 秒心情 -1（吃饱时不衰减）
- 每秒 3.33% 概率自动动作
- 每 5 秒串口：[xxxx ms] EMOTION:x HUNGRY/FULL ACTION | Head:x Feed:x

**摸头**：点击按钮 → 心情 +1（上限受摸头历史影响）→ 触发"玩耍"→ head_count++ 写入 Flash → 串口 Head Touch
- 基础冷却 10 秒，摸头历史多可缩短作为历史记忆影响

**喂食**：编码器绕转一周 → 吃饱状态（120 秒倒计时）→ feed_count++ 写入 Flash → 串口 Feed

**待机**：30 秒无操作 → Deep-sleep → BOOT 键唤醒 → 从 Flash 恢复 → 继续运行

**睡眠**：光敏低光照 5 秒 → OLED 清空，舵机移动到 +10° → 仅按钮唤醒 → 唤醒不回中立位，不触发动作