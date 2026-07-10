修复方案：

```
power/
  power_manager.h / power_manager.cpp
    - 唤醒原因检测（esp_sleep_get_wakeup_cause）
    - GPIO7上的ext0唤醒配置 + 唤醒后的RTC焊盘重新配置
    - enterDeepSleep() — 设置wasSleeping，调用saveToFlash()，进入睡眠
    - 不活动计时器（30秒 → 触发深度睡眠）

storage/
  flash_storage.h / flash_storage.cpp
    - saveToFlash() / loadFromFlash()   （pet_cfg命名空间）
    - incrementPatCount() / incrementFeedCount() / incrementPlayCount()  （pet_mem命名空间）
    - 记忆影响辅助函数（effectiveMoodCap、effectivePatCooldown）
```

两个被精简回退的文件：
- **`pet_state.h`** 
- **`main.cpp`** → 仅调用`power_manager`和`flash_storage`，不内联编写睡眠/Flash逻辑

将睡眠/唤醒代码抽离到`power_manager`中，将Preferences代码抽离到`flash_storage`中，将两者从`pet_state.h`/`main.cpp`中完全剥离，同时在这个过程中，如果唤醒引脚当前在GPIO0上，则将其修复为GPIO7。