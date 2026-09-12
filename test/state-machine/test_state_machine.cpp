#include <cassert>
#include <cstdio>
#include <cstring>
#include "MachineStateMachine.h"

void test_initial_state() {
    MachineStateMachine fsm;
    assert(fsm.getState() == STATE_IDLE);
    assert(fsm.getProfile() == PROFILE_MANUAL);
    assert(fsm.getStage() == STAGE_NONE);
    assert(!fsm.isHeatingActive());
    assert(!fsm.isFaultActive());
    printf("[PASS] test_initial_state\n");
}

void test_manual_heating_flow() {
    MachineStateMachine fsm;
    assert(fsm.set_target_temp(180));
    assert(fsm.getTargetTemp() == 180);

    // Invalid target temp rejected
    assert(!fsm.set_target_temp(350));
    assert(fsm.getTargetTemp() == 180);

    assert(fsm.start_profile(PROFILE_MANUAL));
    assert(fsm.getState() == STATE_HEATING);
    assert(fsm.isHeatingActive());

    // Stop shuts off heater immediately
    fsm.stop();
    assert(fsm.getState() == STATE_IDLE);
    assert(!fsm.isHeatingActive());
    printf("[PASS] test_manual_heating_flow\n");
}

void test_reflow_profile_progression() {
    MachineStateMachine fsm;
    uint32_t now = 1000;

    // Start Lead-Free Profile
    assert(fsm.start_profile(PROFILE_LEAD_FREE));
    assert(fsm.getState() == STATE_HEATING);
    assert(fsm.getStage() == STAGE_PREHEAT);
    assert(fsm.getTargetTemp() == 150);
    assert(fsm.isHeatingActive());

    // Preheat stage
    fsm.update(now, 100.0f, false);
    assert(fsm.getStage() == STAGE_PREHEAT);

    // Reach preheat temp (150°C) -> Advance to Soak
    now += 10000;
    fsm.update(now, 150.0f, false);
    assert(fsm.getState() == STATE_SOAKING);
    assert(fsm.getStage() == STAGE_SOAK);
    assert(fsm.getTargetTemp() == 175);

    // Soak stage dwell (60s soak duration)
    now += 61000;
    fsm.update(now, 175.0f, false);
    assert(fsm.getState() == STATE_REFLOWING);
    assert(fsm.getStage() == STAGE_REFLOW);
    assert(fsm.getTargetTemp() == 245);

    // Reflow peak and dwell (30s dwell)
    now += 5000;
    fsm.update(now, 245.0f, false);
    now += 31000;
    fsm.update(now, 245.0f, false);
    assert(fsm.getState() == STATE_COOLING);
    assert(fsm.getStage() == STAGE_COOL);
    assert(!fsm.isHeatingActive()); // Heater MUST be OFF during cooling

    // Cool down to safe temperature (50°C)
    now += 60000;
    fsm.update(now, 48.0f, false);
    assert(fsm.getState() == STATE_COMPLETE);
    assert(fsm.getProgress() == 1000);
    assert(!fsm.isHeatingActive());

    printf("[PASS] test_reflow_profile_progression\n");
}

void test_pause_and_resume() {
    MachineStateMachine fsm;
    assert(fsm.start_profile(PROFILE_LEAD_FREE));
    assert(fsm.isHeatingActive());

    assert(fsm.pause());
    assert(fsm.getState() == STATE_PAUSED);
    assert(!fsm.isHeatingActive()); // Heating paused

    assert(fsm.resume());
    assert(fsm.getState() == STATE_HEATING);
    assert(fsm.isHeatingActive());

    printf("[PASS] test_pause_and_resume\n");
}

void test_fault_interlocks() {
    MachineStateMachine fsm;
    fsm.start_profile(PROFILE_LEAD_FREE);
    assert(fsm.isHeatingActive());

    // 1. Critical Overtemp fault
    fsm.trigger_fault(FAULT_OVERTEMP, SEVERITY_CRITICAL, "Sensor > 280C");
    assert(fsm.getState() == STATE_FAULT);
    assert(!fsm.isHeatingActive());
    assert(fsm.getFaultCode() == FAULT_OVERTEMP);

    // 2. Guard: Cannot start profile with active fault!
    assert(!fsm.start_profile(PROFILE_LEAD_FREE));
    assert(!fsm.start_profile(PROFILE_MANUAL));

    // 3. Clear fault
    assert(fsm.clear_fault());
    assert(fsm.getState() == STATE_IDLE);
    assert(fsm.getFaultCode() == FAULT_NONE);

    // 4. Communication timeout while heating
    fsm.start_profile(PROFILE_MANUAL);
    assert(fsm.isHeatingActive());
    fsm.update(5000, 100.0f, true /* comm_timed_out */);
    assert(fsm.getState() == STATE_FAULT);
    assert(fsm.getFaultCode() == FAULT_COMM_TIMEOUT);
    assert(!fsm.isHeatingActive());

    printf("[PASS] test_fault_interlocks\n");
}

int main() {
    printf("Running State Machine Unit Tests...\n");
    test_initial_state();
    test_manual_heating_flow();
    test_reflow_profile_progression();
    test_pause_and_resume();
    test_fault_interlocks();
    printf("All State Machine Unit Tests Passed Successfully!\n");
    return 0;
}
