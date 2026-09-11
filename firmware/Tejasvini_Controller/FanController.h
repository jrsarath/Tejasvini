#ifndef TEJASVINI_FAN_CONTROLLER_H_
#define TEJASVINI_FAN_CONTROLLER_H_

#include <stdint.h>
#if __has_include("../shared/Types.h")
#include "../shared/Types.h"
#else
#include "shared/Types.h"
#endif

class FanController {
public:
    FanController(uint8_t pin_fan1_pwm, uint8_t pin_fan2_pwm);

    void init();
    void update(bool is_cooling, float current_temp);

    void setFan1Pwm(uint8_t pwm_pct);
    void setFan2Pwm(uint8_t pwm_pct);
    void setFan1Mode(FanMode mode);
    void setFan2Mode(FanMode mode);

    uint8_t getFan1Pwm() const { return fan1_pwm_; }
    uint8_t getFan2Pwm() const { return fan2_pwm_; }
    FanMode getFan1Mode() const { return fan1_mode_; }
    FanMode getFan2Mode() const { return fan2_mode_; }

private:
    uint8_t pin_fan1_pwm_;
    uint8_t pin_fan2_pwm_;
    uint8_t fan1_pwm_;
    uint8_t fan2_pwm_;
    FanMode fan1_mode_;
    FanMode fan2_mode_;

    void writePwm(uint8_t pin, uint8_t pwm_pct);
};

#endif // TEJASVINI_FAN_CONTROLLER_H_
