#ifndef TEJASVINI_PROTOCOL_CLIENT_H_
#define TEJASVINI_PROTOCOL_CLIENT_H_

#include <stdint.h>
#include <stdbool.h>
#if __has_include("../shared/Config.h")
#include "../shared/Config.h"
#include "../shared/Protocol.h"
#else
#include "shared/Config.h"
#include "shared/Protocol.h"
#endif
#include "StatusModel.h"

class ProtocolClient {
public:
    ProtocolClient(StatusModel& model, uint8_t pin_tx, uint8_t pin_rx, uint32_t baud_rate = UART_BAUD_RATE);

    void init();
    void update(uint32_t now_ms);

    void sendPing();
    void sendGetStatus();
    void sendStartProfile(int profile_id);
    void sendStop();
    void sendPause();
    void sendResume();
    void sendClearFault();
    void sendSetTarget(int target_temp);
    void sendSetFanMode(int fan_id, int mode, float pwm = 0.0f);

    bool isConnected() const { return model_.isConnected(); }

private:
    StatusModel& model_;
    uint8_t pin_tx_;
    uint8_t pin_rx_;
    uint32_t baud_rate_;

    uint32_t next_seq_id_;
    uint32_t last_rx_time_ms_;
    uint32_t last_heartbeat_time_ms_;

    char rx_buffer_[PROTOCOL_MAX_FRAME_LEN * 2];
    size_t rx_idx_;

    void sendCommand(const CommandPacket& cmd);
    void processInboundLine(const char* line);
    bool readLine(char* out_line, size_t max_len);
};

#endif // TEJASVINI_PROTOCOL_CLIENT_H_
