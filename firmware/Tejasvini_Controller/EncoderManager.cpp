#include "EncoderManager.h"
#include "ControllerConfig.h"
#include <Arduino.h>

EncoderManager::EncoderManager(uint8_t pin_a, uint8_t pin_b, uint8_t pin_btn)
    : pin_a_(pin_a), pin_b_(pin_b), pin_btn_(pin_btn),
      last_a_(1), last_b_(1), last_btn_state_(1), btn_press_time_ms_(0),
      accumulated_delta_(0), short_press_flag_(false), long_press_flag_(false) {
}

void EncoderManager::init() {
    pinMode(pin_a_, INPUT_PULLUP);
    pinMode(pin_b_, INPUT_PULLUP);
    pinMode(pin_btn_, INPUT_PULLUP);

    last_a_ = digitalRead(pin_a_);
    last_b_ = digitalRead(pin_b_);
    last_btn_state_ = digitalRead(pin_btn_);
}

void EncoderManager::update() {
    // 1. Button Debounce & Timing (Active LOW)
    int btn = digitalRead(pin_btn_);
    if (btn == 0 && last_btn_state_ == 1) {
        last_btn_state_ = 0;
        btn_press_time_ms_ = millis();
    } else if (btn == 1 && last_btn_state_ == 0) {
        last_btn_state_ = 1;
        uint32_t duration = millis() - btn_press_time_ms_;
        if (duration >= BTN_LONG_PRESS_MS) {
            long_press_flag_ = true;
        } else if (duration >= BTN_SHORT_PRESS_MS) {
            short_press_flag_ = true;
        }
    }

    // 2. Quadrature Decoding
    int a = digitalRead(pin_a_);
    int b = digitalRead(pin_b_);

    if (a != last_a_) {
        if (a == 1) { // Rising edge
            if (b == 0) {
                accumulated_delta_ += 1;
            } else {
                accumulated_delta_ -= 1;
            }
        }
        last_a_ = a;
    }
    last_b_ = b;
}

bool EncoderManager::hasDelta(int& out_delta) {
    if (accumulated_delta_ != 0) {
        out_delta = accumulated_delta_;
        accumulated_delta_ = 0;
        return true;
    }
    return false;
}

bool EncoderManager::isShortPressed() {
    if (short_press_flag_) {
        short_press_flag_ = false;
        return true;
    }
    return false;
}

bool EncoderManager::isLongPressed() {
    if (long_press_flag_) {
        long_press_flag_ = false;
        return true;
    }
    return false;
}
