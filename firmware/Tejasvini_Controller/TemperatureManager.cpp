#include "TemperatureManager.h"
#if __has_include("../shared/Config.h")
#include "../shared/Config.h"
#else
#include "shared/Config.h"
#endif
#include <math.h>

TemperatureManager::TemperatureManager(uint8_t pin_ntc1, uint8_t pin_ntc2, uint8_t pin_ntc3)
    : ntc1_(pin_ntc1), ntc2_(pin_ntc2), ntc3_(pin_ntc3),
      has_ntc3_(pin_ntc3 != 0xFF),
      composite_temp_(25.0f), ntc_delta_(0.0f),
      is_overtemp_(false), divergence_violation_count_(0),
      fault_code_(FAULT_NONE) {
}

void TemperatureManager::init() {
    ntc1_.init();
    ntc2_.init();
    if (has_ntc3_) {
        ntc3_.init();
    }
    composite_temp_ = (ntc1_.getFilteredTemperature() + ntc2_.getFilteredTemperature()) / 2.0f;
    ntc_delta_ = fabsf(ntc1_.getFilteredTemperature() - ntc2_.getFilteredTemperature());
}

bool TemperatureManager::update(bool is_heating) {
    bool ok1 = ntc1_.read();
    bool ok2 = ntc2_.read();
    if (has_ntc3_) {
        ntc3_.read();
    }

    // 1. Check electrical bounds
    if (!ok1) {
        fault_code_ = (ntc1_.getRawAdc() < ADC_MIN_VALID_COUNTS) ? FAULT_NTC1_OPEN : FAULT_NTC1_SHORT;
        return false;
    }
    if (!ok2) {
        fault_code_ = (ntc2_.getRawAdc() < ADC_MIN_VALID_COUNTS) ? FAULT_NTC2_OPEN : FAULT_NTC2_SHORT;
        return false;
    }

    float t1 = ntc1_.getFilteredTemperature();
    float t2 = ntc2_.getFilteredTemperature();
    composite_temp_ = (t1 + t2) / 2.0f;
    ntc_delta_ = fabsf(t1 - t2);

    // 2. Check emergency overtemperature (280°C)
    if (composite_temp_ >= OVERTEMP_SHUTDOWN || t1 >= OVERTEMP_SHUTDOWN || t2 >= OVERTEMP_SHUTDOWN) {
        is_overtemp_ = true;
        fault_code_ = FAULT_OVERTEMP;
        return false;
    }
    is_overtemp_ = false;

    // 3. Sensor divergence checking
    if (!is_heating) {
        divergence_violation_count_ = 0;
        if (ntc_delta_ > MAX_STARTUP_NTC_DIFF) {
            fault_code_ = FAULT_NTC_DIVERGENCE;
            return false;
        }
    } else {
        if (ntc_delta_ > MAX_NTC_DIFF) {
            divergence_violation_count_++;
            if (divergence_violation_count_ >= 5) { // 500ms debounce
                fault_code_ = FAULT_NTC_DIVERGENCE;
                return false;
            }
        } else {
            divergence_violation_count_ = 0;
        }
    }

    fault_code_ = FAULT_NONE;
    return true;
}
