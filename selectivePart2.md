结构形态：

```
network/
  wifi_manager.h/.cpp
    void wifiInit();                      // WiFi.begin()，立即返回
    void wifiUpdate(unsigned long nowMs);  // 间隔门控的状态检查 + 重连
    bool wifiIsConnected();

  weather_client.h/.cpp
    void weatherUpdate(PetState& s, unsigned long nowMs);
      // 由 WEATHER_FETCH_INTERVAL_MS && wifiIsConnected() 门控
      // 内部调用 fetchWeather()，成功时更新 s.cachedWeatherText
    void weatherHandleStatusCode(int httpCode);
      // 集中处理“这个状态码是什么意思” —— 这是你的问答材料，
      // 放在一个地方，而不是分散的内联检查
```

`pet_state.h` 获得你已经用注释占位的小新增：
```cpp
bool wifiConnected = false；
unsigned long lastFetchAttemptMs = 0；
bool lastFetchOk = false；
char cachedWeatherText[32] = "--"；   // 固定缓冲区，不是 String —— 避免
                                       // 长时间运行中的堆碎片化
```

注意:

1. **任何地方都不要有 `while(WiFi.status() != WL_CONNECTED) delay(...)`。** 那是一个阻塞循环，正是我们从第一天起就排除的东西。重连逻辑像其他所有东西一样通过 `wifiUpdate()` 的间隔检查进行。

2. **获取调用本身可以短暂阻塞 —— 这没问题，不是矛盾。** 设置一个明确的 `http.setTimeout(...)`，这样卡住的服务器就不能无限期挂起，并且只每隔几分钟调用一次。在稀有的定时器滴答上有一个有限几秒的暂停，与为某件你没有选修RTOS的东西添加一个FreeRTOS任务相比，是一个公平的权衡。

3. **一个状态码函数，而不是在每个调用点散布内联 `if` 检查** —— 你会被要求解释这些，把答案放在一个你可以指出的地方。

4. **在任何失败情况下**（错误状态码、超时、无WiFi）—— 在屏幕上保留最后缓存的值，不要清空它或崩溃。这 literally 就是“异常时不得卡死”的要求。

成功标准：启动时自动连接，在你开关热点而不触碰板子后自动重连，获取成功并显示真实数据，以及——故意破坏它（关热点、错误URL）——宠物继续正常运行并显示最后的好值，无卡死。