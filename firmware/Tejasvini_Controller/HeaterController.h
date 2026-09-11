#ifndef TEJASVINI_HEATER_CONTROLLER_H_
#define TEJASVINI_HEATER_CONTROLLER_H_

#include <stdint.h>
#include <stdbool.h>

class HeaterController {
public:
    HeaterController(uint8_t pin_ssr, uint8_t pin_relay = 0xFF);

    void init();
    void update(uint32_t now_ms);

    void setDutyCycle(float duty_pct);
    void emergencyOff();

    float getDutyCycle() const { return duty_pct_; }
    bool isOutputHigh() const { return output_high_; }

private:
    uint8_t pin_ssr_;
    uint8_t pin_relay_;
    float duty_pct_;
    bool output_high_;
    uint32_t window_start_ms_;
};

#endif // TEJASVINI_HEATER_CONTROLLER_H_
