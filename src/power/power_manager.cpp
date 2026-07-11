#include "power_manager.h"
#include "flash_storage.h"
#include "servo.h"
#include "pin_config.h"
#include "soc/esp32s3/rtc.h"
#include "driver/rtc_io.h"

RTC_DATA_ATTR uint32_t      bootMs          = 0;
RTC_DATA_ATTR unsigned long lastInteract    = 0;
RTC_DATA_ATTR bool           wasSleeping    = false;
RTC_DATA_ATTR uint64_t      sleepEntryRtcUs = 0;

BootCause detectBootCause() {
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();

    if (cause == ESP_SLEEP_WAKEUP_EXT0) {
        return wasSleeping ? BootCause::SLEEP_WAKE
                           : BootCause::STANDBY_WAKE;
    }

    if (bootMs == 0 && lastInteract == 0) {
        return BootCause::COLD_BOOT;
    }

    return BootCause::COLD_BOOT;
}

void restoreAfterWake(BootCause boot) {
    uint64_t nowRtcUs = esp_rtc_get_time_us();
    unsigned long sleepMs = (nowRtcUs - sleepEntryRtcUs) / 1000;

    loadFromFlash();

    bootMs += sleepMs;

    if (boot == BootCause::SLEEP_WAKE) {
        state.sleepMode = true;
        servoWrite(PIN_SERVO_LEFT, 10);
        servoWrite(PIN_SERVO_RIGHT, 10);
        Serial.println("=== Woke from SLEEP ===");
    } else {
        state.sleepMode = false;
        servoWrite(PIN_SERVO_LEFT, 90);
        servoWrite(PIN_SERVO_RIGHT, 90);

        if (state.isFull) {
            state.fullStateStartMs -= sleepMs;
        }

        unsigned long decay = sleepMs / PetConfig::MOOD_DECAY_INTERVAL_MS;
        if (decay > 0) {
            state.moodValue -= decay;
            if (state.moodValue < PetConfig::MOOD_MIN)
                state.moodValue = PetConfig::MOOD_MIN;
        }

        Serial.println("=== Woke from STANDBY ===");
    }

    state.lastInteractMs = millis();
    lastInteract = bootMs + millis();
}

void enterDeepSleep() {
    saveToFlash();
    sleepEntryRtcUs = esp_rtc_get_time_us();
    bootMs += millis();
    wasSleeping = false;
    rtc_gpio_pullup_en(GPIO_NUM_7);
    rtc_gpio_pulldown_dis(GPIO_NUM_7);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_7, 0);
    esp_deep_sleep_start();
}
