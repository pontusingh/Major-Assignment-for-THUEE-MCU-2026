## 具体引脚表（初稿）

| 外设 | 引脚 | 选此原因 |
|---|---|---|
| 光敏电阻 | GPIO4 | ADC1，通道干净，WiFi 安全 |
| OLED SDA / SCL | GPIO8 / GPIO9 | 空闲，无启动/JTAG/Flash 冲突 |
| 舵机 1（左臂） | GPIO5 | LEDC 任意引脚可用 |
| 舵机 2（右臂） | GPIO6 | 同上 |
| 按键（轻拍） | GPIO7 | 让 ADC1 引脚留出余量 |
| 编码器 A / B | GPIO15 / GPIO16 | 支持 PCNT，远离受限区域 |
| UART0 | GPIO43/44（保留） | 已被 USB/烧录占用——勿动 |
| *（预留）* 超声波 trig/echo | GPIO17 / GPIO18 | 目前空闲 |
| *（预留）* DHT11 | GPIO21 | 目前空闲 |

## 状态管理
采用中心结构体

## 定时策略
在 loop() 中使用 millis() 比较，而非硬件定时器中断。
只把中断（attachInterrupt）保留给两个真正异步的输入：按键边沿和编码器正交脉冲。每个 ISR 只做最少的工作——设置标志或递增计数器——而 loop() 处理所有逻辑。
哪些操作适合 ISR：边沿触发、时序不可预测的输入（按键、编码器）属于 ISR；其他所有内容（衰减、倒计时、显示刷新、自动动作投掷）都属于主循环。

## 睡眠模式下的定时行为
在睡眠期间，情绪衰减、完整倒计时、自动动作投掷和5秒状态打印全部暂停，只有唤醒检测（按键 ISR）保持活跃。


## 本阶段新增文件框架

```
behavior/
  mood.h/.cpp
    void moodUpdate(PetState& s, unsigned long nowMs);
      // 每30秒：mood--（int 转换），下限为1。
      // 【整合睡眠】：如果 s.sleep == SleepState::ASLEEP 或者 s.feeding == FeedingState::FED，则完全跳过。
    void moodApplyPat(PetState& s, unsigned long nowMs);
      // 见冷却时间说明。只有在非休眠状态下被调用。

  feeding.h/.cpp
    void feedingUpdate(PetState& s, unsigned long nowMs);
      // 如果 s.feeding == FeedingState::FED 且 s.sleep == SleepState::AWAKE：
      //   如果 nowMs - feedingStartTime >= FEED_FULL_DURATION_MS：s.feeding = FeedingState::HUNGRY
      // 【整合睡眠】：如果 s.sleep == SleepState::ASLEEP，暂停喂食倒计时。
      //   可通过累加偏移量：feedingStartTime += (nowMs - s.lastLoopTimeMs) 来保持剩余时间不变。
    void feedingTrigger(PetState& s, unsigned long nowMs);
      // 【整合睡眠】：仅在 s.sleep == SleepState::AWAKE 且 encoderCheckFullTurn() 为 true 时调用。
      // s.feeding = FeedingState::FED; feedingStartTime = nowMs（重置倒计时）

  action.h/.cpp
    void actionUpdate(PetState& s, unsigned long nowMs);
      // 【整合睡眠】：如果 s.sleep == SleepState::ASLEEP，直接跳过不执行任何动作更新。
      // 如果 s.action != ActionState::ACTION_NONE 且 nowMs - actionStartTime >= ACTION_DURATION_MS：s.action = ActionState::ACTION_NONE
    void actionTryAutoTrigger(PetState& s, unsigned long nowMs);
      // 【整合睡眠】：如果 s.sleep == SleepState::ASLEEP，直接跳过。
      // 仅在 s.action == ActionState::ACTION_NONE 时；3.33%/秒掷骰；s.mood >= MoodLevel::NEUTRAL -> ActionState::IDLE，s.mood < MoodLevel::NEUTRAL -> ActionState::TIRED
    void actionTrigger(PetState& s, ActionState mode, unsigned long nowMs);
      // 无条件强制触发：停止任何正在运行的动作（即使在运行中），启动新动作。
      // 【整合睡眠】：入睡时会被调用以强行中止当前动作并归零。
    void actionOnPat(PetState& s, unsigned long nowMs);
      // 编排：调用 moodApplyPat(s, nowMs) 然后调用 actionTrigger(s, ActionState::PLAY, nowMs)
      // 【整合睡眠】：此函数仅在非休眠状态下的常规抚摸中被触发。

  sleep.h/.cpp  <-- 【新增睡眠行为模块】
    void sleepUpdate(PetState& s, unsigned long nowMs);
      // 检测光敏电阻。如果持续 5 秒低于暗光阈值且 s.sleep == SleepState::AWAKE：
      //   1. 调用 actionTrigger(s, ActionState::ACTION_NONE, nowMs) 强行中止当前动作。
      //   2. s.sleep = SleepState::ASLEEP 并记录入睡时间戳。
      //   3. 调用驱动层清空并关闭 OLED 屏幕。
      //   4. 控制舵机驱动移至 +80° 休眠位置。
      //   5. 串口输出 "[ms] Sleep Mode On"。
      // 如果光线恢复正常，只重置“持续暗光计时器”，不自动唤醒宠物。
    void sleepWakeUp(PetState& s, unsigned long nowMs);
      // 唤醒行为：
      //   1. s.sleep = SleepState::AWAKE。
      //   2. 控制舵机回到 0° 中位，重新开启并初始化 OLED 显示。
      //   3. 串口输出 "[ms] Sleep Mode Off"。
      // 注意：唤醒不累加心情值，也不触发 Play 动作。
    bool sleepOnButtonPress(PetState& s, unsigned long nowMs);
      // 编排：
      //   如果 s.sleep == SleepState::ASLEEP：
      //     调用 sleepWakeUp(s, nowMs)，并返回 true（表示按键事件已被唤醒拦截，不再往下传递）。
      //   否则：
      //     返回 false（允许 main.cpp 将该按键事件交由常规 actionOnPat 处理）。
```

`main.cpp`的`loop()`保持为一个薄层的调用序列——读取输入，调用各个`*Update()`，
如果`buttonWasPressed()`则调用`actionOnPat()`，然后`oledShowStatus()`。`loop()`本身中没有原始逻辑。

## 两个值得现在固化的正确性陷阱，而不是以后才发现

**1. 即使是在“浪费的”抚摸上，10秒冷却时间也会重置。**
根据规格说明自己的问答：在冷却窗口内再次按下按钮*不会*
增加情绪或重新触发Play，但*确实会*重置10秒计时器。一个朴素的
`if (nowMs - lastPatMs >= PAT_COOLDOWN_MS) { ...应用效果... }` 并且只在那个分支内更新
`lastPatMs`的实现会搞错这一点。正确的形态是：检查
冷却时间与*旧的*`lastPatMs`来决定是否应用情绪+动作效果，
然后无条件地更新`lastPatMs = nowMs`，每次按下都更新，无论结果如何。

**2. 抚摸和自动触发具有不同的“我是否被允许触发”规则。**
自动触发仅在`ActionState::ACTION_NONE`状态下触发。抚摸会*中断*当前正在运行的
*任何*动作，包括正在执行中的自动触发的空闲或疲倦动作。如果你编写一个共享的
“仅在action == ActionState::ACTION_NONE时触发”辅助函数并从两个地方调用它，那么每当一个自动动作恰好正在进行时，抚摸就会悄悄地停止工作。保持`actionTrigger()`
无条件，并把“仅在ActionState::ACTION_NONE时”的条件放在`actionTryAutoTrigger()`
本身中，而不是放在共享函数中。

## 运动系统指南
1.  **引言与概念区分**
    *   区分“外设动作”（硬件单独工作）、“桌宠行为”（多外设联动）与“系统约束”（时机与优先级）。
    *   强调不能让舵机动作卡住整个程序（避免使用阻塞式 `delay()`）。
2.  **舵机在大作业中的使用**
    *   **动作拆分表**：整理各个动作（无动作、玩耍、闲逛、疲惫、前进、睡眠）的触发源、关注问题与验证方式。
    *   **推荐调试顺序**：单舵机 $\rightarrow$ 固定角度 $\rightarrow$ 双舵机 $\rightarrow$ 动作片段 $\rightarrow$ 状态机集成。
    *   **资源占用情况**：2个GPIO、2路LEDC/PWM通道、共用1个LEDC定时器，不占用额外硬件定时器和中断。
3.  **应用层、驱动层与硬件抽象层**
    *   分层职责划分：
        *   **应用层**：决定状态（心情、饱食、动作优先级，解析串口）。
        *   **驱动层**：面向模块的功能接口（设置舵机角度、限制幅值、平滑过渡）。
        *   **硬件抽象层 (HAL)**：初始化和配置外设（PWM通道、GPIO映射、频率配置）。
    *   **输入模块分层**：中断里只记录事件标志（如 `buttonPressed`），耗时操作放回主循环。
4.  **动作打断的实现提示**
    *   **打断的定义**：应用层对当前动作状态重新赋值，新动作立刻覆盖旧动作。
    *   **非阻塞动作更新（核心逻辑）**：保存当前动作编号、开始时间、持续时间。通过当前时间与开始时间的差值，动态计算并更新当前刻的舵机角度。
    *   **事件优先级设置**：进入睡眠（最高） $\rightarrow$ 串口指令 $\rightarrow$ 按钮唤醒 $\rightarrow$ 按钮摸头 $\rightarrow$ 自动动作（最低）。