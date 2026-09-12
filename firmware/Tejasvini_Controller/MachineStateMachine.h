#ifndef TEJASVINI_MACHINE_STATE_MACHINE_H_
#define TEJASVINI_MACHINE_STATE_MACHINE_H_

#include <stdint.h>
#include <stdbool.h>
#include "Config.h"
#include "Types.h"
#include "ErrorCodes.h"

class MachineStateMachine {
public:
    MachineStateMachine();

    void init();
    void update(uint32_t now_ms, float current_temp, bool comm_timed_out);

    // State transitions driven by commands or user input
    bool start_profile(ReflowProfile profile);
    void stop();
    bool pause();
    bool resume();
    bool set_target_temp(int temp);
    bool clear_fault();
    void trigger_fault(FaultCode code, FaultSeverity severity, const char* reason = nullptr);

    // Accessors
    MachineState getState() const { return state_; }
    ReflowProfile getProfile() const { return profile_; }
    ProfileStage getStage() const { return stage_; }
    int getTargetTemp() const { return target_temp_; }
    int32_t getProgress() const { return progress_; }
    uint32_t getElapsedSec() const { return elapsed_s_; }
    uint32_t getTargetSec() const { return target_s_; }
    FaultCode getFaultCode() const { return fault_code_; }
    FaultSeverity getFaultSeverity() const { return fault_severity_; }
    const char* getFaultReason() const { return fault_reason_; }

    bool isHeatingActive() const;
    bool isFaultActive() const { return state_ == STATE_FAULT; }
    bool isCoolingActive() const { return state_ == STATE_COOLING; }

private:
    MachineState state_;
    ReflowProfile profile_;
    ProfileStage stage_;
    int target_temp_;
    int32_t progress_;

    uint32_t stage_start_time_ms_;
    uint32_t elapsed_s_;
    uint32_t target_s_;
    float stage_start_temp_;
    float manual_start_temp_;

    // Pause tracking
    uint32_t pause_start_time_ms_;
    uint32_t accumulated_paused_ms_;
    MachineState pre_pause_state_;

    // Fault state
    FaultCode fault_code_;
    FaultSeverity fault_severity_;
    char fault_reason_[64];

    void advanceToStage(ProfileStage new_stage, uint32_t now_ms, float current_temp);
    void updateManualMode(float current_temp);
    void updateReflowMode(uint32_t now_ms, float current_temp);
};

#endif // TEJASVINI_MACHINE_STATE_MACHINE_H_
