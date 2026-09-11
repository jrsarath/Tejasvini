#include "ProtocolClient.h"
#include "../shared/Serialization.h"
#include <Arduino.h>
#include <string.h>

#define UI_HEARTBEAT_INTERVAL_MS 1000
#define UI_COMM_DISCONNECT_MS   2500

ProtocolClient::ProtocolClient(StatusModel& model, uint8_t pin_tx, uint8_t pin_rx, uint32_t baud_rate)
    : model_(model), pin_tx_(pin_tx), pin_rx_(pin_rx), baud_rate_(baud_rate),
      next_seq_id_(1), last_rx_time_ms_(0), last_heartbeat_time_ms_(0), rx_idx_(0) {
    memset(rx_buffer_, 0, sizeof(rx_buffer_));
}

void ProtocolClient::init() {
    Serial.begin(baud_rate_);
    Serial1.setTX(pin_tx_);
    Serial1.setRX(pin_rx_);
    Serial1.begin(baud_rate_);

    last_rx_time_ms_ = millis();
    last_heartbeat_time_ms_ = millis();
    model_.setConnectionStatus(false);

    sendPing();
}

bool ProtocolClient::readLine(char* out_line, size_t max_len) {
    if (!out_line || max_len == 0) return false;

    Stream* stream = &Serial1;
    if (!Serial1.available() && Serial.available()) {
        stream = &Serial;
    } else if (!Serial1.available()) {
        return false;
    }

    while (stream->available()) {
        int c = stream->read();
        if (c < 0) break;

        if (c == '\r') continue;

        if (c == '\n') {
            if (rx_idx_ > 0) {
                rx_buffer_[rx_idx_] = '\0';
                strncpy(out_line, rx_buffer_, max_len - 1);
                out_line[max_len - 1] = '\0';
                rx_idx_ = 0;
                return true;
            }
            continue;
        }

        if (rx_idx_ < sizeof(rx_buffer_) - 1) {
            rx_buffer_[rx_idx_++] = (char)c;
        } else {
            rx_idx_ = 0;
        }
    }
    return false;
}

void ProtocolClient::sendCommand(const CommandPacket& cmd) {
    char buf[128];
    if (serialize_command(&cmd, buf, sizeof(buf)) > 0) {
        Serial1.print(buf);
        Serial.print(buf);
    }
}

void ProtocolClient::sendPing() {
    CommandPacket cmd;
    memset(&cmd, 0, sizeof(CommandPacket));
    cmd.seq_id = next_seq_id_++;
    cmd.type = CMD_PING;
    sendCommand(cmd);
}

void ProtocolClient::sendGetStatus() {
    CommandPacket cmd;
    memset(&cmd, 0, sizeof(CommandPacket));
    cmd.seq_id = next_seq_id_++;
    cmd.type = CMD_GET_STATUS;
    sendCommand(cmd);
}

void ProtocolClient::sendStartProfile(int profile_id) {
    CommandPacket cmd;
    memset(&cmd, 0, sizeof(CommandPacket));
    cmd.seq_id = next_seq_id_++;
    cmd.type = CMD_START_PROFILE;
    cmd.int_arg1 = profile_id;
    sendCommand(cmd);
}

void ProtocolClient::sendStop() {
    CommandPacket cmd;
    memset(&cmd, 0, sizeof(CommandPacket));
    cmd.seq_id = next_seq_id_++;
    cmd.type = CMD_STOP;
    sendCommand(cmd);
}

void ProtocolClient::sendPause() {
    CommandPacket cmd;
    memset(&cmd, 0, sizeof(CommandPacket));
    cmd.seq_id = next_seq_id_++;
    cmd.type = CMD_PAUSE;
    sendCommand(cmd);
}

void ProtocolClient::sendResume() {
    CommandPacket cmd;
    memset(&cmd, 0, sizeof(CommandPacket));
    cmd.seq_id = next_seq_id_++;
    cmd.type = CMD_RESUME;
    sendCommand(cmd);
}

void ProtocolClient::sendClearFault() {
    CommandPacket cmd;
    memset(&cmd, 0, sizeof(CommandPacket));
    cmd.seq_id = next_seq_id_++;
    cmd.type = CMD_CLEAR_FAULT;
    sendCommand(cmd);
}

void ProtocolClient::sendSetTarget(int target_temp) {
    CommandPacket cmd;
    memset(&cmd, 0, sizeof(CommandPacket));
    cmd.seq_id = next_seq_id_++;
    cmd.type = CMD_SET_TARGET;
    cmd.int_arg1 = target_temp;
    sendCommand(cmd);
}

void ProtocolClient::sendSetFanMode(int fan_id, int mode, float pwm) {
    CommandPacket cmd;
    memset(&cmd, 0, sizeof(CommandPacket));
    cmd.seq_id = next_seq_id_++;
    cmd.type = CMD_SET_FAN_MODE;
    cmd.int_arg1 = fan_id;
    cmd.int_arg2 = mode;
    cmd.float_arg1 = pwm;
    sendCommand(cmd);
}

void ProtocolClient::processInboundLine(const char* line) {
    if (!line || *line == '\0') return;

    last_rx_time_ms_ = millis();
    model_.setConnectionStatus(true);

    if (strncmp(line, "STATUS", 6) == 0) {
        StatusPacket status;
        if (protocol_parse_status(line, &status)) {
            model_.updateFromStatus(status);
        }
    } else if (strncmp(line, "RESP", 4) == 0 || strncmp(line, "PONG", 4) == 0) {
        ResponsePacket resp;
        protocol_parse_response(line, &resp);
    } else if (strncmp(line, "FAULT", 5) == 0) {
        FaultPacket fault;
        protocol_parse_fault(line, &fault);
    }
}

void ProtocolClient::update(uint32_t now_ms) {
    char line[PROTOCOL_MAX_FRAME_LEN * 2];
    while (readLine(line, sizeof(line))) {
        processInboundLine(line);
    }

    // Connection watchdog
    if (now_ms - last_rx_time_ms_ >= UI_COMM_DISCONNECT_MS) {
        model_.setConnectionStatus(false);
    }

    // Heartbeat: query status every 1000ms
    if (now_ms - last_heartbeat_time_ms_ >= UI_HEARTBEAT_INTERVAL_MS) {
        last_heartbeat_time_ms_ = now_ms;
        sendGetStatus();
    }
}
