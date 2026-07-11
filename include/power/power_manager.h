#pragma once
#include <Arduino.h>
#include "esp_sleep.h"
#include "pet_state.h"

// ---- RTC Memory (survives deep-sleep) ----
extern RTC_DATA_ATTR uint32_t     bootMs;
extern RTC_DATA_ATTR unsigned long lastInteract;
extern RTC_DATA_ATTR bool          wasSleeping;
extern RTC_DATA_ATTR uint64_t      sleepEntryRtcUs;

// ---- Boot detection ----
enum class BootCause : uint8_t {
    COLD_BOOT,
    STANDBY_WAKE,
    SLEEP_WAKE
};

BootCause detectBootCause();
void restoreAfterWake(BootCause boot);
void enterDeepSleep();
