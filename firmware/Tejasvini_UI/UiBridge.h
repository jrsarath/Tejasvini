#ifndef TEJASVINI_UI_BRIDGE_H_
#define TEJASVINI_UI_BRIDGE_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Bridge functions called from C (EEZ Studio UI events in actions.c and vars.h)
void ui_bridge_set_target_temp(int temp);
void ui_bridge_change_target_temp(int delta);
void ui_bridge_toggle_heating();
void ui_bridge_cycle_profile();
void ui_bridge_select_profile(int profile_index);

// Variable query bridge functions
const char* ui_bridge_get_status();
const char* ui_bridge_get_target_temp_str();
int32_t ui_bridge_get_target_temp_val();
const char* ui_bridge_get_current_temp_str();
int32_t ui_bridge_get_current_temp_val();
const char* ui_bridge_get_heater_state();
const char* ui_bridge_get_heater_status();
const char* ui_bridge_get_output_percentage_str();
int32_t ui_bridge_get_output_percentage_val();
const char* ui_bridge_get_profile_str();
int32_t ui_bridge_get_profile_index();
int32_t ui_bridge_get_progress();
const char* ui_bridge_get_stage_time();
const char* ui_bridge_get_stage_target();
const char* ui_bridge_get_profile_time();
const char* ui_bridge_get_uptime();
const char* ui_bridge_get_heating_button_str();
const char* ui_bridge_get_ntc_status();
const char* ui_bridge_get_controller();
const char* ui_bridge_get_build();
const char* ui_bridge_get_heater_icon();

#ifdef __cplusplus
}
#endif

#endif // TEJASVINI_UI_BRIDGE_H_
