#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include "Protocol.h"
#include "Serialization.h"
#include "MachineStateMachine.h"

// Mock Controller Harness for Host-Side Integration Testing
class MockControllerHarness {
public:
    MockControllerHarness() : last_duty_(0.0f) {
        fsm_.init();
    }

    std::string processInboundCommand(const char* line) {
        CommandPacket cmd;
        if (!protocol_parse_command(line, &cmd)) {
            ResponsePacket nack;
            nack.seq_id = 0;
            nack.is_ack = false;
            strncpy(nack.message, "INVALID_COMMAND", sizeof(nack.message));
            char buf[128];
            serialize_response(&nack, buf, sizeof(buf));
            return std::string(buf);
        }

        switch (cmd.type) {
            case CMD_PING: {
                ResponsePacket pong;
                pong.seq_id = cmd.seq_id;
                pong.is_ack = true;
                strncpy(pong.message, "PONG", sizeof(pong.message));
                char buf[128];
                serialize_response(&pong, buf, sizeof(buf));
                return std::string(buf);
            }

            case CMD_GET_STATUS: {
                StatusPacket status;
                memset(&status, 0, sizeof(status));
                status.protocol_version = TEJASVINI_PROTOCOL_VERSION;
                strncpy(status.firmware_version, TEJASVINI_FIRMWARE_VERSION, sizeof(status.firmware_version));
                status.state = fsm_.getState();
                status.profile = fsm_.getProfile();
                status.stage = fsm_.getStage();
                status.progress = fsm_.getProgress();
                status.target_temp = fsm_.getTargetTemp();
                status.heater_on = fsm_.isHeatingActive();
                status.duty_cycle = last_duty_;
                status.fault_code = fsm_.getFaultCode();
                char buf[256];
                serialize_status(&status, buf, sizeof(buf));
                return std::string(buf);
            }

            case CMD_START_PROFILE: {
                ReflowProfile prof = (ReflowProfile)cmd.int_arg1;
                ResponsePacket resp;
                resp.seq_id = cmd.seq_id;
                if (fsm_.start_profile(prof)) {
                    resp.is_ack = true;
                    strncpy(resp.message, "STARTED", sizeof(resp.message));
                    last_duty_ = 35.0f; // Simulated active duty
                } else {
                    resp.is_ack = false;
                    strncpy(resp.message, "CANNOT_START", sizeof(resp.message));
                }
                char buf[128];
                serialize_response(&resp, buf, sizeof(buf));
                return std::string(buf);
            }

            case CMD_STOP: {
                fsm_.stop();
                last_duty_ = 0.0f; // Heater forced OFF
                ResponsePacket resp;
                resp.seq_id = cmd.seq_id;
                resp.is_ack = true;
                strncpy(resp.message, "STOPPED", sizeof(resp.message));
                char buf[128];
                serialize_response(&resp, buf, sizeof(buf));
                return std::string(buf);
            }

            default:
                break;
        }

        ResponsePacket nack;
        nack.seq_id = cmd.seq_id;
        nack.is_ack = false;
        strncpy(nack.message, "UNKNOWN_COMMAND", sizeof(nack.message));
        char buf[128];
        serialize_response(&nack, buf, sizeof(buf));
        return std::string(buf);
    }

    void simulateUartTimeout() {
        fsm_.update(10000, 150.0f, true /* comm_timed_out */);
        if (!fsm_.isHeatingActive()) {
            last_duty_ = 0.0f;
        }
    }

    void simulateCriticalNtcFault() {
        fsm_.trigger_fault(FAULT_NTC1_OPEN, SEVERITY_CRITICAL, "NTC1 open circuit");
        last_duty_ = 0.0f;
    }

    MachineStateMachine& getFsm() { return fsm_; }
    float getDuty() const { return last_duty_; }

private:
    MachineStateMachine fsm_;
    float last_duty_;
};

int main() {
    printf("Running End-to-End Protocol & Safety Simulation Tests...\n");
    MockControllerHarness controller;

    // Test 1: PING -> PONG
    {
        std::string reply = controller.processInboundCommand("CMD 1 PING");
        assert(reply.find("RESP 1 ACK PONG") != std::string::npos);
        printf("[PASS] PING -> PONG\n");
    }

    // Test 2: GET_STATUS -> STATUS
    {
        std::string reply = controller.processInboundCommand("CMD 2 GET_STATUS");
        assert(reply.find("STATUS ") != std::string::npos);
        assert(reply.find("state=IDLE") != std::string::npos);
        printf("[PASS] GET_STATUS -> STATUS\n");
    }

    // Test 3: START_PROFILE -> ACK / status transition
    {
        std::string reply = controller.processInboundCommand("CMD 3 START_PROFILE 1");
        assert(reply.find("RESP 3 ACK STARTED") != std::string::npos);
        assert(controller.getFsm().getState() == STATE_HEATING);
        assert(controller.getFsm().getStage() == STAGE_PREHEAT);
        assert(controller.getFsm().isHeatingActive());
        assert(controller.getDuty() > 0.0f);
        printf("[PASS] START_PROFILE -> ACK/status transition\n");
    }

    // Test 4: STOP -> heater OFF
    {
        std::string reply = controller.processInboundCommand("CMD 4 STOP");
        assert(reply.find("RESP 4 ACK STOPPED") != std::string::npos);
        assert(controller.getFsm().getState() == STATE_IDLE);
        assert(!controller.getFsm().isHeatingActive());
        assert(controller.getDuty() == 0.0f);
        printf("[PASS] STOP -> heater OFF\n");
    }

    // Test 5: Invalid command -> NACK
    {
        std::string reply = controller.processInboundCommand("CMD 5 MALFORMED_CRASH_CMD 999");
        assert(reply.find("RESP 0 NACK INVALID_COMMAND") != std::string::npos);
        printf("[PASS] Invalid command -> NACK\n");
    }

    // Test 6: UART timeout during heating -> heater OFF
    {
        controller.processInboundCommand("CMD 6 START_PROFILE 1");
        assert(controller.getFsm().isHeatingActive());
        assert(controller.getDuty() > 0.0f);

        controller.simulateUartTimeout();
        assert(controller.getFsm().getState() == STATE_FAULT);
        assert(controller.getFsm().getFaultCode() == FAULT_COMM_TIMEOUT);
        assert(!controller.getFsm().isHeatingActive());
        assert(controller.getDuty() == 0.0f); // Heater forced OFF!
        printf("[PASS] UART timeout during heating -> heater OFF\n");
    }

    // Test 7: Critical NTC fault -> FAULT and heater OFF
    {
        controller.getFsm().clear_fault();
        controller.processInboundCommand("CMD 7 START_PROFILE 2");
        assert(controller.getFsm().isHeatingActive());

        controller.simulateCriticalNtcFault();
        assert(controller.getFsm().getState() == STATE_FAULT);
        assert(controller.getFsm().getFaultCode() == FAULT_NTC1_OPEN);
        assert(!controller.getFsm().isHeatingActive());
        assert(controller.getDuty() == 0.0f); // Heater forced OFF!
        printf("[PASS] Critical NTC fault -> FAULT and heater OFF\n");
    }

    printf("All End-to-End Simulation Tests Passed Successfully!\n");
    return 0;
}
