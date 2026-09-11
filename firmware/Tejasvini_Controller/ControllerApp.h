#ifndef TEJASVINI_CONTROLLER_APP_H_
#define TEJASVINI_CONTROLLER_APP_H_

#include "ControllerConfig.h"
#include "UartTransport.h"
#include "HeaterController.h"
#include "TemperatureManager.h"
#include "ThermalManager.h"
#include "FanController.h"
#include "Tachometer.h"
#include "EncoderManager.h"
#include "BuzzerManager.h"
#include "ArgbManager.h"
#include "MachineStateMachine.h"
#include "StatusPublisher.h"
#include "CommandParser.h"

class ControllerApp {
public:
    ControllerApp();

    void setup();
    void loop();

private:
    UartTransport transport_;
    HeaterController heater_;
    TemperatureManager temp_mgr_;
    ThermalManager thermal_mgr_;
    FanController fan_ctrl_;
    Tachometer tach_;
    EncoderManager encoder_;
    BuzzerManager buzzer_;
    ArgbManager argb_;
    MachineStateMachine fsm_;
    StatusPublisher publisher_;
    CommandParser parser_;

    uint32_t last_control_tick_ms_;
    char rx_line_[PROTOCOL_MAX_FRAME_LEN];

    void processInputs();
    void runThermalSafetyChecks(uint32_t now_ms);
};

#endif // TEJASVINI_CONTROLLER_APP_H_
