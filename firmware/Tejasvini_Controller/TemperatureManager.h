#ifndef TEJASVINI_TEMPERATURE_MANAGER_H_
#define TEJASVINI_TEMPERATURE_MANAGER_H_

#include "NtcSensor.h"
#include "../shared/ErrorCodes.h"

class TemperatureManager {
public:
    TemperatureManager(uint8_t pin_ntc1, uint8_t pin_ntc2, uint8_t pin_ntc3 = 0xFF);

    void init();
    bool update(bool is_heating);

    float getCompositeTemp() const { return composite_temp_; }
    float getNtc1Temp() const { return ntc1_.getFilteredTemperature(); }
    float getNtc2Temp() const { return ntc2_.getFilteredTemperature(); }
    float getNtc3Temp() const { return has_ntc3_ ? ntc3_.getFilteredTemperature() : -99.0f; }
    float getNtcDelta() const { return ntc_delta_; }

    int getNtc1RawAdc() const { return ntc1_.getRawAdc(); }
    int getNtc2RawAdc() const { return ntc2_.getRawAdc(); }
    int getNtc3RawAdc() const { return has_ntc3_ ? ntc3_.getRawAdc() : 0; }

    float getNtc1Resistance() const { return ntc1_.getResistance(); }
    float getNtc2Resistance() const { return ntc2_.getResistance(); }

    bool hasOvertemp() const { return is_overtemp_; }
    FaultCode getActiveFault() const { return fault_code_; }

private:
    NtcSensor ntc1_;
    NtcSensor ntc2_;
    NtcSensor ntc3_;
    bool has_ntc3_;

    float composite_temp_;
    float ntc_delta_;
    bool is_overtemp_;
    int divergence_violation_count_;
    FaultCode fault_code_;
};

#endif // TEJASVINI_TEMPERATURE_MANAGER_H_
