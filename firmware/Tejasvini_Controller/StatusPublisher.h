#ifndef TEJASVINI_STATUS_PUBLISHER_H_
#define TEJASVINI_STATUS_PUBLISHER_H_

#include "UartTransport.h"
#include "MachineStateMachine.h"
#include "TemperatureManager.h"
#include "ThermalManager.h"
#include "FanController.h"
#include "Tachometer.h"
#if __has_include("../shared/Protocol.h")
#include "../shared/Protocol.h"
#else
#include "shared/Protocol.h"
#endif

class StatusPublisher {
public:
    StatusPublisher(UartTransport& transport,
                    const MachineStateMachine& fsm,
                    const TemperatureManager& temp_mgr,
                    const ThermalManager& thermal_mgr,
                    const FanController& fan_ctrl,
                    const Tachometer& tach);

    void publishNow();
    void update(uint32_t now_ms);

    void sendAck(uint32_t seq_id, const char* message = nullptr);
    void sendNack(uint32_t seq_id, const char* reason);
    void sendPong(uint32_t seq_id);
    void sendFault(FaultCode code, FaultSeverity severity, const char* description);

private:
    UartTransport& transport_;
    const MachineStateMachine& fsm_;
    const TemperatureManager& temp_mgr_;
    const ThermalManager& thermal_mgr_;
    const FanController& fan_ctrl_;
    const Tachometer& tach_;

    uint32_t last_publish_ms_;
};

#endif // TEJASVINI_STATUS_PUBLISHER_H_
