#include "FanController.h"
#if __has_include("../shared/Config.h")
#include "../shared/Config.h"
#else
#include "shared/Config.h"
#endif
#include <Arduino.h>

FanController::FanController(uint8_t pin_fan1_pwm, uint8_t pin_fan2_pwm)
    : pin_fan1_pwm_(pin_fan1_pwm), pin_fan2_pwm_(pin_fan2_pwm),
      fan1_pwm_(0), fan2_pwm_(0),
      fan1_mode_(FAN_MODE_AUTO), fan2_mode_(FAN_MODE_AUTO) {
}

void FanController::init() {
    pinMode(pin_fan1_pwm_, OUTPUT);
    pinMode(pin_fan2_pwm_, OUTPUT);
    writePwm(pin_fan1_pwm_, 0);
    writePwm(pin_fan2_pwm_, 0);
    fan1_pwm_ = 0;
    fan2_pwm_ = 0;
}

void FanController::writePwm(uint8_t pin, uint8_t pwm_pct) {
    if (pwm_pct > 100) pwm_pct = 100;
    // Map 0 - 100% to 8-bit analogWrite (0 - 255)
    int val = (int)((pwm_pct / 100.0f) * 255.0f);
    analogWrite(pin, val);
}

void FanController::setFan1Pwm(uint8_t pwm_pct) {
    if (pwm_pct > 100) pwm_pct = 100;
    fan1_pwm_ = pwm_pct;
    writePwm(pin_fan1_pwm_, fan1_pwm_);
}

void FanController::setFan2Pwm(uint8_t pwm_pct) {
    if (pwm_pct > 100) pwm_pct = 100;
    fan2_pwm_ = pwm_pct;
    writePwm(pin_fan2_pwm_, fan2_pwm_);
}

void FanController::setFan1Mode(FanMode mode) {
    fan1_mode_ = mode;
    if (fan1_mode_ == FAN_MODE_OFF) {
        setFan1Pwm(0);
    }
}

void FanController::setFan2Mode(FanMode mode) {
    fan2_mode_ = mode;
    if (fan2_mode_ == FAN_MODE_OFF) {
        setFan2Pwm(0);
    }
}

void FanController::update(bool is_cooling, float current_temp) {
    (void)current_temp;
    // Auto Mode: Run full blast when in cooling stage, otherwise idle
    if (fan1_mode_ == FAN_MODE_AUTO) {
        uint8_t target_pwm = is_cooling ? FAN_COOLING_PWM : FAN_DEFAULT_IDLE_PWM;
        if (fan1_pwm_ != target_pwm) {
            setFan1Pwm(target_pwm);
        }
    }
    if (fan2_mode_ == FAN_MODE_AUTO) {
        uint8_t target_pwm = is_cooling ? FAN_COOLING_PWM : FAN_DEFAULT_IDLE_PWM;
        if (fan2_pwm_ != target_pwm) {
            setFan2Pwm(target_pwm);
        }
    }
}
