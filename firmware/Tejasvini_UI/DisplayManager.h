#ifndef TEJASVINI_DISPLAY_MANAGER_H_
#define TEJASVINI_DISPLAY_MANAGER_H_

#include "UiConfig.h"
#include "TouchManager.h"
#include <Arduino.h>

class DisplayManager {
public:
    explicit DisplayManager(TouchManager& touch);

    void init();
    void update();

private:
    TouchManager& touch_;
};

#endif // TEJASVINI_DISPLAY_MANAGER_H_
