#include "MachineStateMachine.h"
#include "ProfileEngine.h"
#include <string.h>

MachineStateMachine::MachineStateMachine() {
    init();
}

void MachineStateMachine::init() {
    state_ = STATE_IDLE;
    profile_ = PROFILE_MANUAL;
    stage_ = STAGE_NONE;
    target_temp_ = 0;
    progress_ = 0;
    stage_start_time_ms_ = 0;
    elapsed_s_ = 0;
    target_s_ = 0;
    stage_start_temp_ = 25.0f;
    manual_start_temp_ = 25.0f;
    pause_start_time_ms_ = 0;
    accumulated_paused_ms_ = 0;
    pre_pause_state_ = STATE_IDLE;
    fault_code_ = FAULT_NONE;
    fault_severity_ = SEVERITY_NONE;
    fault_reason_[0] = '\0';
}

bool MachineStateMachine::isHeatingActive() const {
    if (state_ == STATE_FAULT || state_ == STATE_IDLE || state_ == STATE_COMPLETE || state_ == STATE_COOLING) {
        return false;
    }
    return (state_ == STATE_HEATING || state_ == STATE_SOAKING || state_ == STATE_REFLOWING);
}

bool MachineStateMachine::start_profile(ReflowProfile profile) {
    // Guards: Cannot start while in FAULT or BOOTING
    if (state_ == STATE_FAULT || state_ == STATE_BOOTING) {
        return false;
    }

    profile_ = profile;
    progress_ = 0;
    elapsed_s_ = 0;
    accumulated_paused_ms_ = 0;

    if (profile_ == PROFILE_MANUAL) {
        state_ = STATE_HEATING;
        stage_ = STAGE_NONE;
        if (target_temp_ < MIN_SETPOINT_TEMP || target_temp_ == 0) {
            target_temp_ = 100; // Sensible default if unset
        }
        return true;
    }

    // Automated 4-stage reflow profile
    const ProfileStageConfig& cfg = ProfileEngine::getConfig(profile_);
    state_ = STATE_HEATING;
    advanceToStage(STAGE_PREHEAT, 0, 25.0f);
    target_temp_ = cfg.preheat_temp;
    target_s_ = cfg.preheat_time_s;
    return true;
}

void MachineStateMachine::stop() {
    if (state_ == STATE_FAULT) {
        return; // Fault requires explicit clear_fault()
    }
    state_ = STATE_IDLE;
    stage_ = STAGE_NONE;
    progress_ = 0;
    elapsed_s_ = 0;
    target_s_ = 0;
}

bool MachineStateMachine::pause() {
    if (!isHeatingActive()) {
        return false; // Can only pause active heating
    }
    pre_pause_state_ = state_;
    state_ = STATE_PAUSED;
    pause_start_time_ms_ = 0; // Will be set on first tick
    return true;
}

bool MachineStateMachine::resume() {
    if (state_ != STATE_PAUSED) {
        return false;
    }
    state_ = pre_pause_state_;
    return true;
}

bool MachineStateMachine::set_target_temp(int temp) {
    if (temp < MIN_SETPOINT_TEMP || temp > MAX_SETPOINT_TEMP) {
        return false;
    }
    target_temp_ = temp;
    return true;
}

bool MachineStateMachine::clear_fault() {
    if (state_ != STATE_FAULT) {
        return false;
    }
    fault_code_ = FAULT_NONE;
    fault_severity_ = SEVERITY_NONE;
    fault_reason_[0] = '\0';
    state_ = STATE_IDLE;
    stage_ = STAGE_NONE;
    progress_ = 0;
    return true;
}

void MachineStateMachine::trigger_fault(FaultCode code, FaultSeverity severity, const char* reason) {
    state_ = STATE_FAULT;
    fault_code_ = code;
    fault_severity_ = severity;
    if (reason) {
        strncpy(fault_reason_, reason, sizeof(fault_reason_) - 1);
        fault_reason_[sizeof(fault_reason_) - 1] = '\0';
    } else {
        fault_reason_[0] = '\0';
    }
}

void MachineStateMachine::advanceToStage(ProfileStage new_stage, uint32_t now_ms, float current_temp) {
    stage_ = new_stage;
    stage_start_time_ms_ = now_ms;
    stage_start_temp_ = current_temp;
    elapsed_s_ = 0;

    const ProfileStageConfig& cfg = ProfileEngine::getConfig(profile_);
    switch (stage_) {
        case STAGE_PREHEAT:
            state_ = STATE_HEATING;
            target_temp_ = cfg.preheat_temp;
            target_s_ = cfg.preheat_time_s;
            break;
        case STAGE_SOAK:
            state_ = STATE_SOAKING;
            target_temp_ = cfg.soak_temp;
            target_s_ = cfg.soak_time_s;
            break;
        case STAGE_REFLOW:
            state_ = STATE_REFLOWING;
            target_temp_ = cfg.peak_temp;
            target_s_ = cfg.peak_dwell_s;
            break;
        case STAGE_COOL:
            state_ = STATE_COOLING;
            target_temp_ = cfg.cool_temp;
            target_s_ = cfg.cool_time_s;
            break;
        case STAGE_DONE:
            state_ = STATE_COMPLETE;
            target_temp_ = 0;
            target_s_ = 0;
            progress_ = 1000;
            break;
        default:
            break;
    }
}

void MachineStateMachine::updateManualMode(float current_temp) {
    if (state_ != STATE_HEATING) {
        progress_ = 0;
        return;
    }

    float span = (float)target_temp_ - manual_start_temp_;
    if (span <= 1.0f) {
        progress_ = (current_temp >= (float)target_temp_) ? 100 : 0;
    } else {
        float pct = ((current_temp - manual_start_temp_) / span) * 100.0f;
        if (pct < 0.0f) pct = 0.0f;
        if (pct > 100.0f) pct = 100.0f;
        progress_ = (int32_t)(pct + 0.5f);
    }
}

void MachineStateMachine::updateReflowMode(uint32_t now_ms, float current_temp) {
    if (state_ == STATE_IDLE || state_ == STATE_COMPLETE) {
        return;
    }

    if (stage_start_time_ms_ == 0) {
        stage_start_time_ms_ = now_ms;
        stage_start_temp_ = current_temp;
    }

    elapsed_s_ = (now_ms >= stage_start_time_ms_) ? ((now_ms - stage_start_time_ms_) / 1000) : 0;
    const ProfileStageConfig& cfg = ProfileEngine::getConfig(profile_);

    switch (stage_) {
        case STAGE_PREHEAT: {
            // Stage 0: progress 0 - 250
            float span = (float)cfg.preheat_temp - stage_start_temp_;
            float p = (span > 1.0f) ? ((current_temp - stage_start_temp_) / span) : 1.0f;
            if (p < 0.0f) p = 0.0f;
            if (p > 1.0f) p = 1.0f;
            progress_ = (int32_t)(p * 250.0f);

            if (current_temp >= (float)cfg.preheat_temp - 1.0f) {
                advanceToStage(STAGE_SOAK, now_ms, current_temp);
            }
            break;
        }

        case STAGE_SOAK: {
            // Stage 1: progress 250 - 500
            float p = (cfg.soak_time_s > 0) ? ((float)elapsed_s_ / (float)cfg.soak_time_s) : 1.0f;
            if (p < 0.0f) p = 0.0f;
            if (p > 1.0f) p = 1.0f;
            progress_ = 250 + (int32_t)(p * 250.0f);

            if (elapsed_s_ >= cfg.soak_time_s) {
                advanceToStage(STAGE_REFLOW, now_ms, current_temp);
            }
            break;
        }

        case STAGE_REFLOW: {
            // Stage 2: progress 500 - 750
            if (current_temp < (float)cfg.peak_temp - 2.0f) {
                float span = (float)cfg.peak_temp - stage_start_temp_;
                float p = (span > 1.0f) ? ((current_temp - stage_start_temp_) / span) : 1.0f;
                if (p < 0.0f) p = 0.0f;
                if (p > 1.0f) p = 1.0f;
                progress_ = 500 + (int32_t)(p * 175.0f);
            } else {
                float p = (cfg.peak_dwell_s > 0) ? ((float)elapsed_s_ / (float)cfg.peak_dwell_s) : 1.0f;
                if (p < 0.0f) p = 0.0f;
                if (p > 1.0f) p = 1.0f;
                progress_ = 675 + (int32_t)(p * 75.0f);

                if (elapsed_s_ >= cfg.peak_dwell_s) {
                    advanceToStage(STAGE_COOL, now_ms, current_temp);
                }
            }
            break;
        }

        case STAGE_COOL: {
            // Stage 3: progress 750 - 1000
            float span = stage_start_temp_ - (float)cfg.cool_temp;
            float cooled = stage_start_temp_ - current_temp;
            float p = (span > 1.0f) ? (cooled / span) : 1.0f;
            if (p < 0.0f) p = 0.0f;
            if (p > 1.0f) p = 1.0f;
            progress_ = 750 + (int32_t)(p * 250.0f);

            if (current_temp <= (float)cfg.cool_temp) {
                advanceToStage(STAGE_DONE, now_ms, current_temp);
            }
            break;
        }

        case STAGE_DONE:
            state_ = STATE_COMPLETE;
            progress_ = 1000;
            break;

        default:
            break;
    }
}

void MachineStateMachine::update(uint32_t now_ms, float current_temp, bool comm_timed_out) {
    if (comm_timed_out && isHeatingActive()) {
        trigger_fault(FAULT_COMM_TIMEOUT, SEVERITY_CRITICAL, "UART communication timeout during active heating");
        return;
    }

    if (state_ == STATE_FAULT || state_ == STATE_BOOTING || state_ == STATE_PAUSED) {
        return;
    }

    if (profile_ == PROFILE_MANUAL) {
        updateManualMode(current_temp);
    } else {
        updateReflowMode(now_ms, current_temp);
    }
}
