#include "StatusPublisher.h"
#include "Serialization.h"
#include <Arduino.h>

StatusPublisher::StatusPublisher(UartTransport& transport,
                                 const MachineStateMachine& fsm,
                                 const TemperatureManager& temp_mgr,
                                 const ThermalManager& thermal_mgr,
                                 const FanController& fan_ctrl,
                                 const Tachometer& tach)
    : transport_(transport), fsm_(fsm), temp_mgr_(temp_mgr),
      thermal_mgr_(thermal_mgr), fan_ctrl_(fan_ctrl), tach_(tach),
      last_publish_ms_(0) {
}

void StatusPublisher::publishNow() {
    StatusPacket pkt;
    memset(&pkt, 0, sizeof(StatusPacket));
    pkt.protocol_version = TEJASVINI_PROTOCOL_VERSION;
    strncpy(pkt.firmware_version, TEJASVINI_FIRMWARE_VERSION, sizeof(pkt.firmware_version) - 1);
    pkt.state = fsm_.getState();
    pkt.profile = fsm_.getProfile();
    pkt.stage = fsm_.getStage();
    pkt.progress = fsm_.getProgress();
    pkt.elapsed_s = fsm_.getElapsedSec();
    pkt.target_s = fsm_.getTargetSec();
    pkt.target_temp = fsm_.getTargetTemp();
    pkt.ntc1_temp = temp_mgr_.getNtc1Temp();
    pkt.ntc2_temp = temp_mgr_.getNtc2Temp();
    pkt.ntc3_temp = temp_mgr_.getNtc3Temp();
    pkt.heater_on = fsm_.isHeatingActive();
    pkt.duty_cycle = thermal_mgr_.getDutyCycle();
    pkt.fan1_pwm = fan_ctrl_.getFan1Pwm();
    pkt.fan1_rpm = tach_.getFan1Rpm();
    pkt.fan2_pwm = fan_ctrl_.getFan2Pwm();
    pkt.fan2_rpm = tach_.getFan2Rpm();
    pkt.fault_code = fsm_.getFaultCode();
    pkt.fault_severity = fsm_.getFaultSeverity();
    pkt.uptime_s = millis() / 1000UL;

    char buf[256];
    if (serialize_status(&pkt, buf, sizeof(buf)) > 0) {
        transport_.write(buf);
    }
}

void StatusPublisher::update(uint32_t now_ms) {
    if (now_ms - last_publish_ms_ >= COMM_STATUS_PERIOD_MS) {
        last_publish_ms_ = now_ms;
        publishNow();
    }
}

void StatusPublisher::sendAck(uint32_t seq_id, const char* message) {
    ResponsePacket resp;
    resp.seq_id = seq_id;
    resp.is_ack = true;
    if (message) {
        strncpy(resp.message, message, sizeof(resp.message) - 1);
    } else {
        resp.message[0] = '\0';
    }
    char buf[128];
    if (serialize_response(&resp, buf, sizeof(buf)) > 0) {
        transport_.write(buf);
    }
}

void StatusPublisher::sendNack(uint32_t seq_id, const char* reason) {
    ResponsePacket resp;
    resp.seq_id = seq_id;
    resp.is_ack = false;
    strncpy(resp.message, reason ? reason : "ERROR", sizeof(resp.message) - 1);
    char buf[128];
    if (serialize_response(&resp, buf, sizeof(buf)) > 0) {
        transport_.write(buf);
    }
}

void StatusPublisher::sendPong(uint32_t seq_id) {
    sendAck(seq_id, "PONG");
}

void StatusPublisher::sendFault(FaultCode code, FaultSeverity severity, const char* description) {
    FaultPacket pkt;
    pkt.code = code;
    pkt.severity = severity;
    strncpy(pkt.description, description ? description : "Fault", sizeof(pkt.description) - 1);
    char buf[128];
    if (serialize_fault(&pkt, buf, sizeof(buf)) > 0) {
        transport_.write(buf);
    }
}
