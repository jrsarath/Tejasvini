#include "TouchManager.h"
#include "UiConfig.h"
#include <Arduino.h>
#include <Wire.h>
#include <TAMC_GT911.h>

static TAMC_GT911 ts(TOUCH_GT911_SDA, TOUCH_GT911_SCL, TOUCH_GT911_INT,
                     TOUCH_GT911_RST, 800, 480);

TouchManager::TouchManager() : last_x_(0), last_y_(0) {
}

void TouchManager::init() {
    ts.begin();
    ts.setRotation(ROTATION_NORMAL);
}

bool TouchManager::hasSignal() {
    return true;
}

bool TouchManager::isTouched() {
    ts.read();
    if (ts.isTouched) {
        int raw_x = ts.points[0].x;
        int raw_y = ts.points[0].y;

        int physical_x = map(800 - raw_x, 0, 800, 0, DISPLAY_WIDTH - 1);
        int physical_y = map(480 - raw_y, 0, 480, 0, DISPLAY_HEIGHT - 1);

        // 270° rotation to match 240x400 portrait space
        last_x_ = (DISPLAY_HEIGHT - 1) - physical_y;
        last_y_ = physical_x;

        last_x_ = constrain(last_x_, 0, SCREEN_WIDTH - 1);
        last_y_ = constrain(last_y_, 0, SCREEN_HEIGHT - 1);

        return true;
    }
    return false;
}

bool TouchManager::isReleased() {
    return true;
}
