#ifndef TEJASVINI_WATCHDOG_MANAGER_H_
#define TEJASVINI_WATCHDOG_MANAGER_H_

#include <stdint.h>
#include <stdbool.h>

class WatchdogManager {
public:
    static void init(uint32_t timeout_ms = 8000);
    static void feed();
    static bool causedReboot();
};

#endif // TEJASVINI_WATCHDOG_MANAGER_H_
