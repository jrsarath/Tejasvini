#include "ArgbManager.h"
#include <Arduino.h>

ArgbManager::ArgbManager(uint8_t pin_led)
    : pin_led_(pin_led), r_(0), g_(0), b_(0), last_anim_ms_(0), anim_phase_(0) {
}

void ArgbManager::init() {
    pinMode(pin_led_, OUTPUT);
    digitalWrite(pin_led_, LOW);
}

void ArgbManager::setColor(uint8_t r, uint8_t g, uint8_t b) {
    r_ = r;
    g_ = g;
    b_ = b;
    // Basic digital toggle or PWM indicator on standalone pin
    if (r_ > 0 || g_ > 0 || b_ > 0) {
        digitalWrite(pin_led_, HIGH);
    } else {
        digitalWrite(pin_led_, LOW);
    }
}

void ArgbManager::update(uint32_t now_ms, MachineState state) {
    if (now_ms - last_anim_ms_ < 100) {
        return;
    }
    last_anim_ms_ = now_ms;
    anim_phase_++;

    switch (state) {
        case STATE_BOOTING:
            setColor(0, 150, 200); // Cyan
            break;
        case STATE_IDLE:
            setColor(0, 180, 0);   // Green
            break;
        case STATE_HEATING:
        case STATE_SOAKING:
            setColor(220, 100, 0); // Orange / Amber
            break;
        case STATE_REFLOWING:
            setColor(255, 0, 0);   // Red
            break;
        case STATE_COOLING:
            setColor(0, 80, 255);  // Blue
            break;
        case STATE_PAUSED:
            setColor(180, 180, 0); // Yellow
            break;
        case STATE_COMPLETE:
            setColor(0, 255, 120); // Bright Mint / Green
            break;
        case STATE_FAULT:
            // Fast red strobe
            if (anim_phase_ % 2 == 0) {
                setColor(255, 0, 0);
            } else {
                setColor(0, 0, 0);
            }
            break;
    }
}
