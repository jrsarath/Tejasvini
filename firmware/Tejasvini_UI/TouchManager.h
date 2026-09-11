#ifndef TEJASVINI_TOUCH_MANAGER_H_
#define TEJASVINI_TOUCH_MANAGER_H_

#include <stdint.h>
#include <stdbool.h>

class TouchManager {
public:
    TouchManager();

    void init();
    bool hasSignal();
    bool isTouched();
    bool isReleased();

    int getLastX() const { return last_x_; }
    int getLastY() const { return last_y_; }

private:
    int last_x_;
    int last_y_;
};

#endif // TEJASVINI_TOUCH_MANAGER_H_
