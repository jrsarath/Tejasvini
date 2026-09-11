#include "UiBridge.h"
#include "UiApp.h"
#include <string.h>

void ui_bridge_set_target_temp(int temp) {
    UiApp* app = UiApp::getInstance();
    if (app) {
        app->getStatusModel().setRequestedTargetTemp(temp);
        app->getProtocolClient().sendSetTarget(temp);
    }
}

void ui_bridge_change_target_temp(int delta) {
    UiApp* app = UiApp::getInstance();
    if (app) {
        int current = app->getStatusModel().getTargetTempVal();
        int new_target = current + delta;
        if (new_target < MIN_SETPOINT_TEMP) new_target = MIN_SETPOINT_TEMP;
        if (new_target > MAX_SETPOINT_TEMP) new_target = MAX_SETPOINT_TEMP;
        ui_bridge_set_target_temp(new_target);
    }
}

void ui_bridge_toggle_heating() {
    UiApp* app = UiApp::getInstance();
    if (app) {
        if (app->getStatusModel().isHeatingActive()) {
            app->getProtocolClient().sendStop();
        } else if (app->getStatusModel().isFault()) {
            app->getProtocolClient().sendClearFault();
        } else {
            int prof = app->getStatusModel().getProfileIndex();
            app->getProtocolClient().sendStartProfile(prof);
        }
    }
}

void ui_bridge_cycle_profile() {
    UiApp* app = UiApp::getInstance();
    if (app) {
        int current = app->getStatusModel().getProfileIndex();
        int next = (current + 1) % 4;
        ui_bridge_select_profile(next);
    }
}

void ui_bridge_select_profile(int profile_index) {
    UiApp* app = UiApp::getInstance();
    if (app) {
        if (app->getStatusModel().isHeatingActive()) {
            app->getProtocolClient().sendStop();
        }
        app->getStatusModel().setSelectedProfileIndex(profile_index);
        static const int preset_temps[4] = {0, 245, 215, 150};
        if (profile_index >= 0 && profile_index < 4 && preset_temps[profile_index] > 0) {
            ui_bridge_set_target_temp(preset_temps[profile_index]);
        }
    }
}

// Queries
const char* ui_bridge_get_status() {
    UiApp* app = UiApp::getInstance();
    return app ? app->getStatusModel().getStatusStr() : "IDLE";
}

const char* ui_bridge_get_target_temp_str() {
    UiApp* app = UiApp::getInstance();
    return app ? app->getStatusModel().getTargetTempStr() : "0";
}

int32_t ui_bridge_get_target_temp_val() {
    UiApp* app = UiApp::getInstance();
    return app ? app->getStatusModel().getTargetTempVal() : 0;
}

const char* ui_bridge_get_current_temp_str() {
    UiApp* app = UiApp::getInstance();
    return app ? app->getStatusModel().getCurrentTempStr() : "0";
}

int32_t ui_bridge_get_current_temp_val() {
    UiApp* app = UiApp::getInstance();
    return app ? app->getStatusModel().getCurrentTempVal() : 0;
}

const char* ui_bridge_get_heater_state() {
    UiApp* app = UiApp::getInstance();
    return app ? app->getStatusModel().getHeaterStateStr() : "STANDBY";
}

const char* ui_bridge_get_heater_status() {
    UiApp* app = UiApp::getInstance();
    return app ? app->getStatusModel().getHeaterStatusStr() : "Standby";
}

const char* ui_bridge_get_output_percentage_str() {
    UiApp* app = UiApp::getInstance();
    return app ? app->getStatusModel().getOutputPercentageStr() : "0%";
}

int32_t ui_bridge_get_output_percentage_val() {
    UiApp* app = UiApp::getInstance();
    return app ? app->getStatusModel().getOutputPercentageVal() : 0;
}

const char* ui_bridge_get_profile_str() {
    UiApp* app = UiApp::getInstance();
    return app ? app->getStatusModel().getProfileStr() : "MANUAL";
}

int32_t ui_bridge_get_profile_index() {
    UiApp* app = UiApp::getInstance();
    return app ? app->getStatusModel().getProfileIndex() : 0;
}

int32_t ui_bridge_get_progress() {
    UiApp* app = UiApp::getInstance();
    return app ? app->getStatusModel().getProgressVal() : 0;
}

const char* ui_bridge_get_stage_time() {
    UiApp* app = UiApp::getInstance();
    return app ? app->getStatusModel().getStageTimeStr() : "N/A";
}

const char* ui_bridge_get_stage_target() {
    UiApp* app = UiApp::getInstance();
    return app ? app->getStatusModel().getStageTargetStr() : "N/A";
}

const char* ui_bridge_get_profile_time() {
    UiApp* app = UiApp::getInstance();
    return app ? app->getStatusModel().getProfileTimeStr() : "IDLE";
}

const char* ui_bridge_get_uptime() {
    UiApp* app = UiApp::getInstance();
    return app ? app->getStatusModel().getUptimeStr() : "00:00:00";
}

const char* ui_bridge_get_heating_button_str() {
    UiApp* app = UiApp::getInstance();
    return app ? app->getStatusModel().getHeatingButtonStr() : "\uf04b  START HEATING";
}

const char* ui_bridge_get_ntc_status() {
    UiApp* app = UiApp::getInstance();
    return app ? app->getStatusModel().getNtcStatusStr() : "OK";
}

const char* ui_bridge_get_controller() {
    UiApp* app = UiApp::getInstance();
    return app ? app->getStatusModel().getControllerStr() : "RP2350B";
}

const char* ui_bridge_get_build() {
    UiApp* app = UiApp::getInstance();
    return app ? app->getStatusModel().getBuildStr() : "v1.1.0";
}

const char* ui_bridge_get_heater_icon() {
    UiApp* app = UiApp::getInstance();
    return app ? app->getStatusModel().getHeaterIconStr() : "\uf251";
}
