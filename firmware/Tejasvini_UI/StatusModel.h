#ifndef TEJASVINI_STATUS_MODEL_H_
#define TEJASVINI_STATUS_MODEL_H_

#include <stdint.h>
#include <stdbool.h>
#include "../shared/Protocol.h"

class StatusModel {
public:
    StatusModel();

    void updateFromStatus(const StatusPacket& pkt);
    void setConnectionStatus(bool connected);

    // Getters for UI Variables
    const char* getStatusStr() const;
    const char* getTargetTempStr() const;
    int32_t getTargetTempVal() const;
    const char* getCurrentTempStr() const;
    int32_t getCurrentTempVal() const;
    const char* getHeaterStateStr() const;
    const char* getHeaterStatusStr() const;
    const char* getOutputPercentageStr() const;
    int32_t getOutputPercentageVal() const;
    const char* getProfileStr() const;
    int32_t getProfileIndex() const;
    int32_t getProgressVal() const;
    const char* getStageTimeStr() const;
    const char* getStageTargetStr() const;
    const char* getProfileTimeStr() const;
    const char* getUptimeStr() const;
    const char* getHeatingButtonStr() const;
    const char* getNtcStatusStr() const;
    const char* getControllerStr() const;
    const char* getBuildStr() const;
    const char* getHeaterIconStr() const;

    bool isHeatingActive() const;
    bool isConnected() const { return is_connected_; }
    bool isFault() const { return latest_status_.state == STATE_FAULT; }

    // User interaction requests
    void setSelectedProfileIndex(int32_t idx);
    void setRequestedTargetTemp(int32_t temp);

private:
    StatusPacket latest_status_;
    bool is_connected_;
    int32_t requested_profile_idx_;
    int32_t requested_target_temp_;

    mutable char buf_status_[32];
    mutable char buf_target_temp_[16];
    mutable char buf_current_temp_[16];
    mutable char buf_heater_state_[16];
    mutable char buf_heater_status_[32];
    mutable char buf_output_pct_[16];
    mutable char buf_stage_time_[16];
    mutable char buf_stage_target_[16];
    mutable char buf_profile_time_[16];
    mutable char buf_uptime_[32];
    mutable char buf_btn_str_[32];
    mutable char buf_ntc_status_[16];
    mutable char buf_heater_icon_[16];

    void formatDuration(char* buf, size_t size, uint32_t total_sec) const;
};

#endif // TEJASVINI_STATUS_MODEL_H_
