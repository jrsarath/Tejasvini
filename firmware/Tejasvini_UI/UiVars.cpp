#include "UiBridge.h"
#include "ui/vars.h"
#include <stdlib.h>
#include <string.h>

static int32_t s_selected_tab = 0;

extern "C" {

// Status
const char *get_var_status() { return ui_bridge_get_status(); }
void set_var_status(const char *value) { (void)value; }

// Target Temperature String
const char *get_var_target_temp_str() { return ui_bridge_get_target_temp_str(); }
void set_var_target_temp_str(const char *value) {
    if (value) {
        ui_bridge_set_target_temp(atoi(value));
    }
}

// Target Temperature Numeric
int32_t get_var_target_temp_val() { return ui_bridge_get_target_temp_val(); }
void set_var_target_temp_val(int32_t value) { ui_bridge_set_target_temp((int)value); }

// Current Temperature String
const char *get_var_current_temp_str() { return ui_bridge_get_current_temp_str(); }
void set_var_current_temp_str(const char *value) { (void)value; }

// Current Temperature Numeric
int32_t get_var_current_temp_val() { return ui_bridge_get_current_temp_val(); }
void set_var_current_temp_val(int32_t value) { (void)value; }

// Heater State
const char *get_var_heater_state() { return ui_bridge_get_heater_state(); }
void set_var_heater_state(const char *value) { (void)value; }

// Heater Status
const char *get_var_heater_status() { return ui_bridge_get_heater_status(); }
void set_var_heater_status(const char *value) { (void)value; }

// Output Percentage String
const char *get_var_output_percentage_str() { return ui_bridge_get_output_percentage_str(); }
void set_var_output_percentage_str(const char *value) { (void)value; }

// Output Percentage Numeric
int32_t get_var_output_percentage_val() { return ui_bridge_get_output_percentage_val(); }
void set_var_output_percentage_val(int32_t value) { (void)value; }

// Profile String
const char *get_var_profile_str() { return ui_bridge_get_profile_str(); }
void set_var_profile_str(const char *value) {
    if (value) {
        static const char* names[4] = {"MANUAL", "LEAD FREE", "LEADED", "LOW TEMP"};
        for (int i = 0; i < 4; i++) {
            if (strcmp(value, names[i]) == 0) {
                ui_bridge_select_profile(i);
                break;
            }
        }
    }
}

// Profile Index
int32_t get_var_profile_index() { return ui_bridge_get_profile_index(); }
void set_var_profile_index(int32_t value) { ui_bridge_select_profile((int)value); }

// Progress (0 - 1000)
int32_t get_var_progress() { return ui_bridge_get_progress(); }
void set_var_progress(int32_t value) { (void)value; }

// Stage Time
const char *get_var_stage_time() { return ui_bridge_get_stage_time(); }
void set_var_stage_time(const char *value) { (void)value; }

// Stage Target
const char *get_var_stage_target() { return ui_bridge_get_stage_target(); }
void set_var_stage_target(const char *value) { (void)value; }

// Profile Time
const char *get_var_profile_time() { return ui_bridge_get_profile_time(); }
void set_var_profile_time(const char *value) { (void)value; }

// Uptime
const char *get_var_uptime() { return ui_bridge_get_uptime(); }
void set_var_uptime(const char *value) { (void)value; }

// Heating Button
const char *get_var_heating_button_str() { return ui_bridge_get_heating_button_str(); }
void set_var_heating_button_str(const char *value) { (void)value; }

// NTC Status
const char *get_var_ntc_status() { return ui_bridge_get_ntc_status(); }
void set_var_ntc_status(const char *value) { (void)value; }

// Controller Info
const char *get_var_controller() { return ui_bridge_get_controller(); }
void set_var_controller(const char *value) { (void)value; }

// Build Info
const char *get_var_build() { return ui_bridge_get_build(); }
void set_var_build(const char *value) { (void)value; }

// Heater Icon
const char *get_var_heater_icon() { return ui_bridge_get_heater_icon(); }
void set_var_heater_icon(const char *value) { (void)value; }

// Navigation Tabs
bool get_var_nav_home() { return s_selected_tab == 0; }
void set_var_nav_home(bool value) { if (value) s_selected_tab = 0; }

bool get_var_nav_profiles() { return s_selected_tab == 1; }
void set_var_nav_profiles(bool value) { if (value) s_selected_tab = 1; }

bool get_var_nav_manual() { return s_selected_tab == 2; }
void set_var_nav_manual(bool value) { if (value) s_selected_tab = 2; }

bool get_var_nav_info() { return s_selected_tab == 3; }
void set_var_nav_info(bool value) { if (value) s_selected_tab = 3; }

} // extern "C"
