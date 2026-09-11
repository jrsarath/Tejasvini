#ifndef TEJASVINI_ENCODER_MANAGER_H_
#define TEJASVINI_ENCODER_MANAGER_H_

#include <stdint.h>
#include <stdbool.h>

class EncoderManager {
public:
    EncoderManager(uint8_t pin_a, uint8_t pin_b, uint8_t pin_btn);

    void init();
    void update();

    // Callback triggers for events
    bool hasDelta(int& out_delta);
    bool isShortPressed();
    bool isLongPressed();

private:
    uint8_t pin_a_;
    uint8_t pin_b_;
    uint8_t pin_btn_;

    int last_a_;
    int last_b_;
    int last_btn_state_;
    uint32_t btn_press_time_ms_;

    int accumulated_delta_;
    bool short_press_flag_;
    bool long_press_flag_;
};

#endif // TEJASVINI_ENCODER_MANAGER_H_
