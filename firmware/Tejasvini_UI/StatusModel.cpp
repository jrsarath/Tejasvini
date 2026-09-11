#include "StatusModel.h"
#include <stdio.h>
#include <string.h>

StatusModel::StatusModel()
    : is_connected_(false),
      requested_profile_idx_(0),
      requested_target_temp_(0) {
    memset(&latest_status_, 0, sizeof(StatusPacket));
    latest_status_.protocol_version = TEJASVINI_PROTOCOL_VERSION;
    latest_status_.state = STATE_IDLE;
    latest_status_.profile = PROFILE_MANUAL;
    latest_status_.ntc1_temp = 25.0f;
    latest_status_.ntc2_temp = 25.0f;
    latest_status_.ntc3_temp = -99.0f;
}

void StatusModel::updateFromStatus(const StatusPacket& pkt) {
    latest_status_ = pkt;
    is_connected_ = true;
    requested_profile_idx_ = (int32_t)latest_status_.profile;
    requested_target_temp_ = latest_status_.target_temp;
}

void StatusModel::setConnectionStatus(bool connected) {
    is_connected_ = connected;
}

void StatusModel::setSelectedProfileIndex(int32_t idx) {
    if (idx >= 0 && idx < 4) {
        requested_profile_idx_ = idx;
    }
}

void StatusModel::setRequestedTargetTemp(int32_t temp) {
    if (temp >= MIN_SETPOINT_TEMP && temp <= MAX_SETPOINT_TEMP) {
        requested_target_temp_ = temp;
    }
}

void StatusModel::formatDuration(char* buf, size_t size, uint32_t total_sec) const {
    uint32_t sec = total_sec % 60;
    uint32_t min = (total_sec / 60) % 60;
    uint32_t hr = total_sec / 3600;
    if (hr > 0) {
        snprintf(buf, size, "%lu:%02lu:%02lu", (unsigned long)hr, (unsigned long)min, (unsigned long)sec);
    } else {
        snprintf(buf, size, "%lu:%02lu", (unsigned long)min, (unsigned long)sec);
    }
}

const char* StatusModel::getStatusStr() const {
    if (!is_connected_) {
        return "DISCONNECTED";
    }
    if (latest_status_.state == STATE_FAULT) {
        return "ERROR";
    }
    if (latest_status_.profile != PROFILE_MANUAL) {
        switch (latest_status_.stage) {
            case STAGE_PREHEAT: return "PREHEAT";
            case STAGE_SOAK:    return "SOAK";
            case STAGE_REFLOW:  return "REFLOW";
            case STAGE_COOL:    return "COOLING";
            case STAGE_DONE:    return "DONE";
            default: break;
        }
    }
    if (latest_status_.state == STATE_HEATING) {
        return "HEATING";
    }
    return machine_state_to_string(latest_status_.state);
}

const char* StatusModel::getTargetTempStr() const {
    snprintf(buf_target_temp_, sizeof(buf_target_temp_), "%d", (int)requested_target_temp_);
    return buf_target_temp_;
}

int32_t StatusModel::getTargetTempVal() const {
    return requested_target_temp_;
}

const char* StatusModel::getCurrentTempStr() const {
    int temp_int = (int)(latest_status_.ntc1_temp + 0.5f);
    snprintf(buf_current_temp_, sizeof(buf_current_temp_), "%d", temp_int);
    return buf_current_temp_;
}

int32_t StatusModel::getCurrentTempVal() const {
    return (int32_t)(latest_status_.ntc1_temp + 0.5f);
}

const char* StatusModel::getHeaterStateStr() const {
    if (!is_connected_) return "STANDBY";
    if (latest_status_.profile != PROFILE_MANUAL) {
        if (latest_status_.stage == STAGE_COOL) return "COOLING";
        if (latest_status_.stage == STAGE_DONE) return "COMPLETE";
    }
    return latest_status_.heater_on ? "HEATING" : "STANDBY";
}

const char* StatusModel::getHeaterStatusStr() const {
    if (!is_connected_) {
        return "Controller offline";
    }
    if (latest_status_.state == STATE_FAULT) {
        return "Safety trip / Fault";
    }
    if (latest_status_.profile != PROFILE_MANUAL) {
        switch (latest_status_.stage) {
            case STAGE_PREHEAT: return "Stage: Preheating";
            case STAGE_SOAK:    return "Stage: Soaking flux";
            case STAGE_REFLOW:  return "Stage: Reflow peak";
            case STAGE_COOL:    return "Stage: Cooling down";
            case STAGE_DONE:    return "Profile complete!";
            default:            return "Profile at standby";
        }
    }
    if (latest_status_.heater_on) {
        return "Heater is active";
    }
    return "Heater at standby";
}

const char* StatusModel::getOutputPercentageStr() const {
    snprintf(buf_output_pct_, sizeof(buf_output_pct_), "%d%%", (int)(latest_status_.duty_cycle + 0.5f));
    return buf_output_pct_;
}

int32_t StatusModel::getOutputPercentageVal() const {
    return (int32_t)(latest_status_.duty_cycle + 0.5f);
}

const char* StatusModel::getProfileStr() const {
    return reflow_profile_to_string(latest_status_.profile);
}

int32_t StatusModel::getProfileIndex() const {
    return requested_profile_idx_;
}

int32_t StatusModel::getProgressVal() const {
    return latest_status_.progress;
}

const char* StatusModel::getStageTimeStr() const {
    if (latest_status_.profile == PROFILE_MANUAL) return "N/A";
    if (latest_status_.stage == STAGE_NONE) return "IDLE";
    formatDuration(buf_stage_time_, sizeof(buf_stage_time_), latest_status_.elapsed_s);
    return buf_stage_time_;
}

const char* StatusModel::getStageTargetStr() const {
    if (latest_status_.profile == PROFILE_MANUAL) return "N/A";
    if (latest_status_.stage == STAGE_NONE) return "IDLE";
    formatDuration(buf_stage_target_, sizeof(buf_stage_target_), latest_status_.target_s);
    return buf_stage_target_;
}

const char* StatusModel::getProfileTimeStr() const {
    if (!latest_status_.heater_on && latest_status_.stage == STAGE_NONE) return "IDLE";
    formatDuration(buf_profile_time_, sizeof(buf_profile_time_), latest_status_.elapsed_s);
    return buf_profile_time_;
}

const char* StatusModel::getUptimeStr() const {
    formatDuration(buf_uptime_, sizeof(buf_uptime_), latest_status_.uptime_s);
    return buf_uptime_;
}

const char* StatusModel::getHeatingButtonStr() const {
    bool active = isHeatingActive() || (latest_status_.profile != PROFILE_MANUAL && latest_status_.stage == STAGE_COOL);
    if (active) {
        strncpy(buf_btn_str_, "\uf04d  STOP HEATING", sizeof(buf_btn_str_) - 1);
    } else {
        strncpy(buf_btn_str_, "\uf04b  START HEATING", sizeof(buf_btn_str_) - 1);
    }
    buf_btn_str_[sizeof(buf_btn_str_) - 1] = '\0';
    return buf_btn_str_;
}

const char* StatusModel::getNtcStatusStr() const {
    if (!is_connected_ || latest_status_.state == STATE_FAULT) {
        return "ERROR";
    }
    return "OK";
}

const char* StatusModel::getControllerStr() const {
    return "RP2350B";
}

const char* StatusModel::getBuildStr() const {
    return TEJASVINI_FIRMWARE_VERSION;
}

const char* StatusModel::getHeaterIconStr() const {
    bool active = isHeatingActive() || (latest_status_.profile != PROFILE_MANUAL && latest_status_.stage == STAGE_COOL);
    return active ? "\uf3b1" : "\uf251";
}

bool StatusModel::isHeatingActive() const {
    return is_connected_ && (latest_status_.heater_on ||
           latest_status_.state == STATE_HEATING ||
           latest_status_.state == STATE_SOAKING ||
           latest_status_.state == STATE_REFLOWING);
}
