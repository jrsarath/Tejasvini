#ifndef TEJASVINI_NTC_SENSOR_H_
#define TEJASVINI_NTC_SENSOR_H_

#include <stdint.h>
#include <stdbool.h>

class NtcSensor {
public:
    NtcSensor(uint8_t pin, float r_divider = 100000.0f, float r0 = 100000.0f,
              float beta = 3950.0f, float t0_kelvin = 298.15f);

    void init();
    bool read();

    int getRawAdc() const { return raw_adc_; }
    float getResistance() const { return resistance_; }
    float getRawTemperature() const { return raw_temp_; }
    float getFilteredTemperature() const { return filtered_temp_; }

    bool isConnected() const { return is_connected_; }
    bool isFault() const { return is_fault_; }

private:
    uint8_t pin_;
    float r_divider_;
    float r0_;
    float beta_;
    float t0_kelvin_;

    int raw_adc_;
    float resistance_;
    float raw_temp_;
    float filtered_temp_;
    bool is_connected_;
    bool is_fault_;
    bool is_initialized_;

    int sampleOversampledAdc();
};

#endif // TEJASVINI_NTC_SENSOR_H_
