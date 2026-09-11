#include "BuzzerManager.h"
#include <Arduino.h>

BuzzerManager::BuzzerManager(uint8_t pin_buzzer)
    : pin_buzzer_(pin_buzzer), is_playing_(false),
      tone_end_time_ms_(0), alarm_active_(false),
      alarm_toggle_time_ms_(0), alarm_state_(false) {
}

void BuzzerManager::init() {
    pinMode(pin_buzzer_, OUTPUT);
    digitalWrite(pin_buzzer_, LOW);
}

void BuzzerManager::playBeep(uint16_t freq_hz, uint16_t duration_ms) {
    tone(pin_buzzer_, freq_hz, duration_ms);
    is_playing_ = true;
    tone_end_time_ms_ = millis() + duration_ms;
}

void BuzzerManager::playClick() {
    playBeep(2400, 20);
}

void BuzzerManager::playStartTone() {
    playBeep(1800, 150);
}

void BuzzerManager::playCompleteTune() {
    playBeep(2200, 400);
}

void BuzzerManager::playAlarm(bool active) {
    alarm_active_ = active;
    if (!alarm_active_) {
        noTone(pin_buzzer_);
        digitalWrite(pin_buzzer_, LOW);
    }
}

void BuzzerManager::update(uint32_t now_ms) {
    if (alarm_active_) {
        if (now_ms - alarm_toggle_time_ms_ >= 250) {
            alarm_toggle_time_ms_ = now_ms;
            alarm_state_ = !alarm_state_;
            if (alarm_state_) {
                tone(pin_buzzer_, 3000);
            } else {
                tone(pin_buzzer_, 1500);
            }
        }
        return;
    }

    if (is_playing_ && now_ms >= tone_end_time_ms_) {
        noTone(pin_buzzer_);
        digitalWrite(pin_buzzer_, LOW);
        is_playing_ = false;
    }
}
