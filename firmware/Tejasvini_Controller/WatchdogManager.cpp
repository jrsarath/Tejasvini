#include "WatchdogManager.h"
#include <Arduino.h>

#if defined(ARDUINO_ARCH_RP2040)
#include <hardware/watchdog.h>
#endif

void WatchdogManager::init(uint32_t timeout_ms) {
#if defined(ARDUINO_ARCH_RP2040)
    watchdog_enable(timeout_ms, 1);
#else
    (void)timeout_ms;
#endif
}

void WatchdogManager::feed() {
#if defined(ARDUINO_ARCH_RP2040)
    watchdog_update();
#endif
}

bool WatchdogManager::causedReboot() {
#if defined(ARDUINO_ARCH_RP2040)
    return watchdog_caused_reboot();
#else
    return false;
#endif
}
