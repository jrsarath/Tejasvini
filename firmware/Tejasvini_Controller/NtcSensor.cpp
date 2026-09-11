#include "NtcSensor.h"
#include "../shared/Config.h"
#include <Arduino.h>
#include <math.h>

#define OVERSAMPLE_COUNT 32
#define DISCARD_COUNT 6
#define ALPHA_EMA 0.08f
#define MAX_TEMP_SLEW_PER_100MS 0.25f

NtcSensor::NtcSensor(uint8_t pin, float r_divider, float r0, float beta, float t0_kelvin)
    : pin_(pin), r_divider_(r_divider), r0_(r0), beta_(beta), t0_kelvin_(t0_kelvin),
      raw_adc_(0), resistance_(0.0f), raw_temp_(25.0f), filtered_temp_(25.0f),
      is_connected_(false), is_fault_(false), is_initialized_(false) {
}

void NtcSensor::init() {
    pinMode(pin_, INPUT);
    raw_adc_ = sampleOversampledAdc();
    read();
    filtered_temp_ = raw_temp_;
    is_initialized_ = true;
}

int NtcSensor::sampleOversampledAdc() {
    int samples[OVERSAMPLE_COUNT];
    for (int i = 0; i < OVERSAMPLE_COUNT; i++) {
        samples[i] = analogRead(pin_);
        delayMicroseconds(4);
    }

    // In-place insertion sort
    for (int i = 1; i < OVERSAMPLE_COUNT; i++) {
        int key = samples[i];
        int j = i - 1;
        while (j >= 0 && samples[j] > key) {
            samples[j + 1] = samples[j];
            j--;
        }
        samples[j + 1] = key;
    }

    // Average the middle 20 samples
    long sum = 0;
    int valid_count = OVERSAMPLE_COUNT - (2 * DISCARD_COUNT);
    for (int i = DISCARD_COUNT; i < OVERSAMPLE_COUNT - DISCARD_COUNT; i++) {
        sum += samples[i];
    }
    return (int)(sum / valid_count);
}

bool NtcSensor::read() {
    raw_adc_ = sampleOversampledAdc();

    // Check for open circuit / short
    if (raw_adc_ < ADC_MIN_VALID_COUNTS || raw_adc_ > ADC_MAX_VALID_COUNTS) {
        is_fault_ = true;
        is_connected_ = false;
        return false;
    }

    is_connected_ = true;
    is_fault_ = false;

    // Resistance calculation for high-side thermistor divider
    resistance_ = r_divider_ * ((4095.0f / (float)raw_adc_) - 1.0f);

    // Steinhart-Hart / Beta equation
    float steinhart = (1.0f / t0_kelvin_) + (1.0f / beta_) * log(resistance_ / r0_);
    raw_temp_ = (1.0f / steinhart) - 273.15f;

    // Sanity check physical temperature range
    if (raw_temp_ < NTC_MIN_VALID_TEMP || raw_temp_ > NTC_MAX_VALID_TEMP) {
        is_fault_ = true;
        return false;
    }

    if (!is_initialized_) {
        filtered_temp_ = raw_temp_;
        is_initialized_ = true;
        return true;
    }

    // Exponential Moving Average (EMA)
    float new_filtered = (ALPHA_EMA * raw_temp_) + ((1.0f - ALPHA_EMA) * filtered_temp_);

    // Physical slew rate limiter (0.25°C per 100ms)
    float delta = new_filtered - filtered_temp_;
    if (delta > MAX_TEMP_SLEW_PER_100MS) {
        new_filtered = filtered_temp_ + MAX_TEMP_SLEW_PER_100MS;
    } else if (delta < -MAX_TEMP_SLEW_PER_100MS) {
        new_filtered = filtered_temp_ - MAX_TEMP_SLEW_PER_100MS;
    }
    filtered_temp_ = new_filtered;

    return true;
}
