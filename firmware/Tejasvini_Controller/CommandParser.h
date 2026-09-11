#ifndef TEJASVINI_COMMAND_PARSER_H_
#define TEJASVINI_COMMAND_PARSER_H_

#include "MachineStateMachine.h"
#include "ThermalManager.h"
#include "FanController.h"
#include "StatusPublisher.h"
#include "../shared/Protocol.h"

class CommandParser {
public:
    CommandParser(MachineStateMachine& fsm,
                  ThermalManager& thermal_mgr,
                  FanController& fan_ctrl,
                  StatusPublisher& publisher);

    void processLine(const char* line);

private:
    MachineStateMachine& fsm_;
    ThermalManager& thermal_mgr_;
    FanController& fan_ctrl_;
    StatusPublisher& publisher_;

    void handleCommand(const CommandPacket& cmd);
};

#endif // TEJASVINI_COMMAND_PARSER_H_
