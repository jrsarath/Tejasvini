#include "CommandParser.h"
#include <string.h>

CommandParser::CommandParser(MachineStateMachine& fsm,
                             ThermalManager& thermal_mgr,
                             FanController& fan_ctrl,
                             StatusPublisher& publisher)
    : fsm_(fsm), thermal_mgr_(thermal_mgr), fan_ctrl_(fan_ctrl), publisher_(publisher) {
}

void CommandParser::processLine(const char* line) {
    if (!line || *line == '\0') return;

    CommandPacket cmd;
    if (!protocol_parse_command(line, &cmd)) {
        publisher_.sendNack(0, "INVALID_COMMAND");
        return;
    }

    handleCommand(cmd);
}

void CommandParser::handleCommand(const CommandPacket& cmd) {
    switch (cmd.type) {
        case CMD_PING:
            publisher_.sendPong(cmd.seq_id);
            break;

        case CMD_GET_STATUS:
            publisher_.publishNow();
            break;

        case CMD_START_PROFILE: {
            ReflowProfile prof = (ReflowProfile)cmd.int_arg1;
            if (fsm_.start_profile(prof)) {
                publisher_.sendAck(cmd.seq_id, "STARTED");
                publisher_.publishNow();
            } else {
                publisher_.sendNack(cmd.seq_id, "CANNOT_START");
            }
            break;
        }

        case CMD_STOP:
            fsm_.stop();
            thermal_mgr_.emergencyStop();
            publisher_.sendAck(cmd.seq_id, "STOPPED");
            publisher_.publishNow();
            break;

        case CMD_PAUSE:
            if (fsm_.pause()) {
                publisher_.sendAck(cmd.seq_id, "PAUSED");
                publisher_.publishNow();
            } else {
                publisher_.sendNack(cmd.seq_id, "CANNOT_PAUSE");
            }
            break;

        case CMD_RESUME:
            if (fsm_.resume()) {
                publisher_.sendAck(cmd.seq_id, "RESUMED");
                publisher_.publishNow();
            } else {
                publisher_.sendNack(cmd.seq_id, "CANNOT_RESUME");
            }
            break;

        case CMD_CLEAR_FAULT:
            if (fsm_.clear_fault()) {
                thermal_mgr_.reset();
                publisher_.sendAck(cmd.seq_id, "FAULT_CLEARED");
                publisher_.publishNow();
            } else {
                publisher_.sendNack(cmd.seq_id, "NO_ACTIVE_FAULT");
            }
            break;

        case CMD_SET_TARGET:
            if (fsm_.set_target_temp(cmd.int_arg1)) {
                publisher_.sendAck(cmd.seq_id, "TARGET_SET");
                publisher_.publishNow();
            } else {
                publisher_.sendNack(cmd.seq_id, "TARGET_OUT_OF_RANGE");
            }
            break;

        case CMD_SET_FAN_MODE:
            if (cmd.int_arg1 == 1) {
                fan_ctrl_.setFan1Mode((FanMode)cmd.int_arg2);
                if ((FanMode)cmd.int_arg2 == FAN_MODE_MANUAL) {
                    fan_ctrl_.setFan1Pwm((uint8_t)cmd.float_arg1);
                }
                publisher_.sendAck(cmd.seq_id, "FAN1_UPDATED");
            } else if (cmd.int_arg1 == 2) {
                fan_ctrl_.setFan2Mode((FanMode)cmd.int_arg2);
                if ((FanMode)cmd.int_arg2 == FAN_MODE_MANUAL) {
                    fan_ctrl_.setFan2Pwm((uint8_t)cmd.float_arg1);
                }
                publisher_.sendAck(cmd.seq_id, "FAN2_UPDATED");
            } else {
                publisher_.sendNack(cmd.seq_id, "INVALID_FAN_ID");
            }
            break;

        default:
            publisher_.sendNack(cmd.seq_id, "UNKNOWN_COMMAND");
            break;
    }
}
