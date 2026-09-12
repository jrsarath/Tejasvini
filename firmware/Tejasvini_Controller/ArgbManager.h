#ifndef TEJASVINI_ARGB_MANAGER_H_
#define TEJASVINI_ARGB_MANAGER_H_

#include <stdint.h>
#include "Types.h"

class ArgbManager {
public:
    explicit ArgbManager(uint8_t pin_led);

    void init();
    void update(uint32_t now_ms, MachineState state);

    void setColor(uint8_t r, uint8_t g, uint8_t b);

private:
    uint8_t pin_led_;
    uint8_t r_;
    uint8_t g_;
    uint8_t b_;
    uint32_t last_anim_ms_;
    uint8_t anim_phase_;
};

#endif // TEJASVINI_ARGB_MANAGER_H_
