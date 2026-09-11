#include "Tachometer.h"
#include <Arduino.h>

static Tachometer* s_tach_instance = nullptr;

static void isr_fan1_tach() {
    if (s_tach_instance) s_tach_instance->onFan1Pulse();
}

static void isr_fan2_tach() {
    if (s_tach_instance) s_tach_instance->onFan2Pulse();
}

Tachometer::Tachometer(uint8_t pin_tach1, uint8_t pin_tach2)
    : pin_tach1_(pin_tach1), pin_tach2_(pin_tach2),
      pulse_count1_(0), pulse_count2_(0),
      last_calc_time_ms_(0), fan1_rpm_(0), fan2_rpm_(0) {
    s_tach_instance = this;
}

void Tachometer::init() {
    pinMode(pin_tach1_, INPUT_PULLUP);
    pinMode(pin_tach2_, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(pin_tach1_), isr_fan1_tach, FALLING);
    attachInterrupt(digitalPinToInterrupt(pin_tach2_), isr_fan2_tach, FALLING);

    last_calc_time_ms_ = millis();
}

void Tachometer::onFan1Pulse() {
    pulse_count1_++;
}

void Tachometer::onFan2Pulse() {
    pulse_count2_++;
}

void Tachometer::update(uint32_t now_ms) {
    uint32_t dt_ms = now_ms - last_calc_time_ms_;
    if (dt_ms < 1000) {
        return; // Calculate RPM every 1 second
    }

    noInterrupts();
    uint32_t p1 = pulse_count1_;
    uint32_t p2 = pulse_count2_;
    pulse_count1_ = 0;
    pulse_count2_ = 0;
    interrupts();

    // Standard fans output 2 pulses per revolution: RPM = (pulses / 2) * (60000 / dt_ms)
    fan1_rpm_ = (uint16_t)((p1 * 30000UL) / dt_ms);
    fan2_rpm_ = (uint16_t)((p2 * 30000UL) / dt_ms);
    last_calc_time_ms_ = now_ms;
}
