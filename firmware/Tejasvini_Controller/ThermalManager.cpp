#include "ThermalManager.h"
#include "ControllerConfig.h"
#include <Arduino.h>

ThermalManager::ThermalManager(HeaterController& heater)
    : heater_(heater), reference_temp_(25.0f), integrator_acc_(0.0f),
      last_calc_time_ms_(0), runaway_timer_start_ms_(0),
      runaway_start_temp_(25.0f), runaway_monitoring_(false),
      runaway_fault_(false) {
}

void ThermalManager::init() {
    heater_.init();
    reset();
}

void ThermalManager::reset() {
    reference_temp_ = 25.0f;
    integrator_acc_ = 0.0f;
    last_calc_time_ms_ = millis();
    runaway_timer_start_ms_ = 0;
    runaway_monitoring_ = false;
    runaway_fault_ = false;
    heater_.setDutyCycle(0.0f);
}

void ThermalManager::emergencyStop() {
    integrator_acc_ = 0.0f;
    heater_.emergencyOff();
}

void ThermalManager::update(uint32_t now_ms, float measured_temp, int target_temp, bool heating_demanded) {
    if (!heating_demanded || target_temp <= 0) {
        reference_temp_ = measured_temp;
        integrator_acc_ = 0.0f;
        runaway_monitoring_ = false;
        heater_.setDutyCycle(0.0f);
        last_calc_time_ms_ = now_ms;
        return;
    }

    float dt = (now_ms - last_calc_time_ms_) / 1000.0f;
    if (dt <= 0.0f || dt > 1.0f) {
        dt = 0.1f; // Clamp to nominal 100ms
    }
    last_calc_time_ms_ = now_ms;

    // 1. Soft-start reference ramping
    float target_f = (float)target_temp;
    if (reference_temp_ < target_f) {
        reference_temp_ += RAMP_RATE_DEG_PER_S * dt;
        if (reference_temp_ > target_f) reference_temp_ = target_f;
    } else if (reference_temp_ > target_f) {
        reference_temp_ -= RAMP_RATE_DEG_PER_S * dt;
        if (reference_temp_ < target_f) reference_temp_ = target_f;
    }

    // 2. PI Regulation
    float error = reference_temp_ - measured_temp;

    // Anti-windup integration: only accumulate if not saturating in that direction
    float p_term = KP_GAIN * error;
    float projected_duty = p_term + (KI_GAIN * (integrator_acc_ + (error * dt)));

    if (projected_duty >= MIN_DUTY_CYCLE && projected_duty <= MAX_DUTY_CYCLE) {
        integrator_acc_ += error * dt;
    }

    float duty = p_term + (KI_GAIN * integrator_acc_);
    heater_.setDutyCycle(duty);

    // 3. Thermal Runaway Protection
    // When driving significant power (>= 20%) with a temperature deficit (> 10°C),
    // temperature must rise by at least 2.0°C within 18 seconds.
    if (heater_.getDutyCycle() >= 20.0f && (target_f - measured_temp) > 10.0f) {
        if (!runaway_monitoring_) {
            runaway_monitoring_ = true;
            runaway_timer_start_ms_ = now_ms;
            runaway_start_temp_ = measured_temp;
        } else {
            if (now_ms - runaway_timer_start_ms_ >= THERMAL_RUNAWAY_PERIOD_MS) {
                float rise = measured_temp - runaway_start_temp_;
                if (rise < THERMAL_RUNAWAY_MIN_RISE) {
                    runaway_fault_ = true;
                    emergencyStop();
                } else {
                    // Reset interval window
                    runaway_timer_start_ms_ = now_ms;
                    runaway_start_temp_ = measured_temp;
                }
            }
        }
    } else {
        runaway_monitoring_ = false;
    }
}
