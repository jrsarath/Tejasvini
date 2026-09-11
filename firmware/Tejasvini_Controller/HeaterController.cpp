#include "HeaterController.h"
#if __has_include("../shared/Config.h")
#include "../shared/Config.h"
#else
#include "shared/Config.h"
#endif
#include <Arduino.h>

HeaterController::HeaterController(uint8_t pin_ssr, uint8_t pin_relay)
    : pin_ssr_(pin_ssr), pin_relay_(pin_relay),
      duty_pct_(0.0f), output_high_(false), window_start_ms_(0) {
}

void HeaterController::init() {
    pinMode(pin_ssr_, OUTPUT);
    digitalWrite(pin_ssr_, LOW);

    if (pin_relay_ != 0xFF) {
        pinMode(pin_relay_, OUTPUT);
        digitalWrite(pin_relay_, LOW);
    }

    duty_pct_ = 0.0f;
    output_high_ = false;
    window_start_ms_ = millis();
}

void HeaterController::setDutyCycle(float duty_pct) {
    if (duty_pct < MIN_DUTY_CYCLE) {
        duty_pct = MIN_DUTY_CYCLE;
    }
    // CRITICAL: Strict safety clamping to MAX_DUTY_CYCLE (40%)
    if (duty_pct > MAX_DUTY_CYCLE) {
        duty_pct = MAX_DUTY_CYCLE;
    }
    duty_pct_ = duty_pct;

    if (duty_pct_ > 0.0f && pin_relay_ != 0xFF) {
        digitalWrite(pin_relay_, HIGH);
    }
}

void HeaterController::emergencyOff() {
    duty_pct_ = 0.0f;
    output_high_ = false;
    digitalWrite(pin_ssr_, LOW);
    if (pin_relay_ != 0xFF) {
        digitalWrite(pin_relay_, LOW);
    }
}

void HeaterController::update(uint32_t now_ms) {
    if (duty_pct_ <= 0.0f) {
        if (output_high_) {
            output_high_ = false;
            digitalWrite(pin_ssr_, LOW);
        }
        if (pin_relay_ != 0xFF) {
            digitalWrite(pin_relay_, LOW);
        }
        return;
    }

    // SSR Time-Proportioning (1000ms window)
    if (now_ms - window_start_ms_ >= 1000) {
        window_start_ms_ += 1000;
    }

    uint32_t on_time_ms = (uint32_t)((duty_pct_ / 100.0f) * 1000.0f);
    bool should_be_high = (now_ms - window_start_ms_ < on_time_ms);

    if (should_be_high != output_high_) {
        output_high_ = should_be_high;
        digitalWrite(pin_ssr_, output_high_ ? HIGH : LOW);
    }
}
