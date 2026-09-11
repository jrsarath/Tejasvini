#include "ControllerApp.h"
#include "WatchdogManager.h"
#include <Arduino.h>

ControllerApp::ControllerApp()
    : transport_(PIN_UART_TX, PIN_UART_RX, UART_BAUD_RATE),
      heater_(PIN_SSR, PIN_RELAY),
      temp_mgr_(PIN_NTC1, PIN_NTC2, PIN_NTC3),
      thermal_mgr_(heater_),
      fan_ctrl_(PIN_FAN1_PWM, PIN_FAN2_PWM),
      tach_(PIN_FAN1_TACH, PIN_FAN2_TACH),
      encoder_(PIN_ENA, PIN_ENB, PIN_EBT),
      buzzer_(PIN_BUZZER),
      argb_(PIN_ARGB),
      fsm_(),
      publisher_(transport_, fsm_, temp_mgr_, thermal_mgr_, fan_ctrl_, tach_),
      parser_(fsm_, thermal_mgr_, fan_ctrl_, publisher_),
      last_control_tick_ms_(0) {
    rx_line_[0] = '\0';
}

void ControllerApp::setup() {
    // 1. Mandatory safe output initialization
    heater_.emergencyOff();
    fan_ctrl_.init();
    buzzer_.init();
    argb_.init();
    argb_.update(millis(), STATE_BOOTING);

    // 2. Initialize communication
    transport_.init();

    // 3. Initialize temperature sensors & PI regulator
    temp_mgr_.init();
    thermal_mgr_.init();

    // 4. Initialize input & feedback
    encoder_.init();
    tach_.init();
    fsm_.init();

    // 5. Initialize watchdog
    WatchdogManager::init(WATCHDOG_TIMEOUT_MS);

    // Initial audio cue & state update
    buzzer_.playStartTone();
    argb_.update(millis(), STATE_IDLE);

    publisher_.publishNow();
}

void ControllerApp::processInputs() {
    encoder_.update();

    int delta = 0;
    if (encoder_.hasDelta(delta)) {
        int current_target = fsm_.getTargetTemp();
        int new_target = current_target + delta;
        if (new_target < MIN_SETPOINT_TEMP) new_target = MIN_SETPOINT_TEMP;
        if (new_target > MAX_SETPOINT_TEMP) new_target = MAX_SETPOINT_TEMP;
        fsm_.set_target_temp(new_target);
        buzzer_.playClick();
        publisher_.publishNow();
    }

    if (encoder_.isShortPressed()) {
        buzzer_.playClick();
        if (fsm_.isHeatingActive()) {
            fsm_.stop();
            thermal_mgr_.emergencyStop();
        } else if (fsm_.getState() == STATE_IDLE) {
            fsm_.start_profile(PROFILE_MANUAL);
        }
        publisher_.publishNow();
    }

    if (encoder_.isLongPressed()) {
        buzzer_.playBeep(1200, 200);
        if (fsm_.isFaultActive()) {
            fsm_.clear_fault();
            thermal_mgr_.reset();
        }
        publisher_.publishNow();
    }
}

void ControllerApp::runThermalSafetyChecks(uint32_t now_ms) {
    bool ok = temp_mgr_.update(fsm_.isHeatingActive());
    if (!ok) {
        FaultCode code = temp_mgr_.getActiveFault();
        fsm_.trigger_fault(code, SEVERITY_CRITICAL, fault_code_to_string(code));
        thermal_mgr_.emergencyStop();
        publisher_.sendFault(code, SEVERITY_CRITICAL, fault_code_to_string(code));
        buzzer_.playAlarm(true);
        return;
    }

    if (thermal_mgr_.isRunawayFault()) {
        fsm_.trigger_fault(FAULT_THERMAL_RUNAWAY, SEVERITY_CRITICAL, "Thermal runaway detected");
        thermal_mgr_.emergencyStop();
        publisher_.sendFault(FAULT_THERMAL_RUNAWAY, SEVERITY_CRITICAL, "Thermal runaway detected");
        buzzer_.playAlarm(true);
        return;
    }

    if (!fsm_.isFaultActive()) {
        buzzer_.playAlarm(false);
    }
}

void ControllerApp::loop() {
    WatchdogManager::feed();
    uint32_t now = millis();

    // 1. Process inbound UART protocol messages
    while (transport_.readLine(rx_line_, sizeof(rx_line_))) {
        parser_.processLine(rx_line_);
    }

    // 2. Process local rotary encoder and button inputs
    processInputs();

    // 3. Periodic 100ms thermal regulation and safety checks
    if (now - last_control_tick_ms_ >= CONTROL_PERIOD_MS) {
        last_control_tick_ms_ = now;

        // Check UART communication timeout (5 seconds while heating)
        bool comm_timed_out = false;
        if (fsm_.isHeatingActive()) {
            if (now - transport_.getLastRxTimeMs() >= COMM_TIMEOUT_MS) {
                comm_timed_out = true;
            }
        }

        runThermalSafetyChecks(now);

        fsm_.update(now, temp_mgr_.getCompositeTemp(), comm_timed_out);

        if (comm_timed_out && fsm_.isHeatingActive()) {
            thermal_mgr_.emergencyStop();
            publisher_.sendFault(FAULT_COMM_TIMEOUT, SEVERITY_CRITICAL, "UART timeout");
            buzzer_.playAlarm(true);
        }

        thermal_mgr_.update(now, temp_mgr_.getCompositeTemp(), fsm_.getTargetTemp(), fsm_.isHeatingActive());
    }

    // 4. Continuous time-proportioning SSR actuation
    heater_.update(now);

    // 5. Update fans, tachometers, buzzers, and ARGB lighting
    fan_ctrl_.update(fsm_.isCoolingActive(), temp_mgr_.getCompositeTemp());
    tach_.update(now);
    buzzer_.update(now);
    argb_.update(now, fsm_.getState());

    // 6. Periodic telemetry broadcast
    publisher_.update(now);

    yield();
}
