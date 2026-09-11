#ifndef TEJASVINI_THERMAL_MANAGER_H_
#define TEJASVINI_THERMAL_MANAGER_H_

#include "HeaterController.h"
#include "../shared/ErrorCodes.h"
#include <stdint.h>
#include <stdbool.h>

class ThermalManager {
public:
    ThermalManager(HeaterController& heater);

    void init();
    void reset();

    void update(uint32_t now_ms, float measured_temp, int target_temp, bool heating_demanded);

    float getReferenceTemp() const { return reference_temp_; }
    float getDutyCycle() const { return heater_.getDutyCycle(); }
    bool isRunawayFault() const { return runaway_fault_; }

    void emergencyStop();

private:
    HeaterController& heater_;

    float reference_temp_;
    float integrator_acc_;
    uint32_t last_calc_time_ms_;

    // Thermal runaway monitor
    uint32_t runaway_timer_start_ms_;
    float runaway_start_temp_;
    bool runaway_monitoring_;
    bool runaway_fault_;
};

#endif // TEJASVINI_THERMAL_MANAGER_H_
