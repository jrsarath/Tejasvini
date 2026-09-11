#ifndef TEJASVINI_TACHOMETER_H_
#define TEJASVINI_TACHOMETER_H_

#include <stdint.h>

class Tachometer {
public:
    Tachometer(uint8_t pin_tach1, uint8_t pin_tach2);

    void init();
    void update(uint32_t now_ms);

    uint16_t getFan1Rpm() const { return fan1_rpm_; }
    uint16_t getFan2Rpm() const { return fan2_rpm_; }

    void onFan1Pulse();
    void onFan2Pulse();

private:
    uint8_t pin_tach1_;
    uint8_t pin_tach2_;

    volatile uint32_t pulse_count1_;
    volatile uint32_t pulse_count2_;

    uint32_t last_calc_time_ms_;
    uint16_t fan1_rpm_;
    uint16_t fan2_rpm_;
};

#endif // TEJASVINI_TACHOMETER_H_
