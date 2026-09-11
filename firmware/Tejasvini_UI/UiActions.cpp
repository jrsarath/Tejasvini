#include "UiBridge.h"
#include <lvgl.h>
#include <stdint.h>

extern "C" {

/**
 * @brief Sets the target temperature directly via the bridge to controller.
 * @param e LVGL event object carrying the desired temperature in user_data.
 */
void action_set_target_temp(lv_event_t *e) {
  intptr_t target_temp = (intptr_t)lv_event_get_user_data(e);
  if (target_temp > 0) {
    ui_bridge_set_target_temp((int)target_temp);
  }
}

/**
 * @brief Increases the target temperature by delta amount.
 * @param e LVGL event object carrying the increment step (defaults to 1 if 0).
 */
void action_increase_target_temp(lv_event_t *e) {
  intptr_t delta = (intptr_t)lv_event_get_user_data(e);
  int step = (delta != 0) ? (int)delta : 1;
  ui_bridge_change_target_temp(step);
}

/**
 * @brief Decreases the target temperature by delta amount.
 * @param e LVGL event object carrying the decrement step (defaults to 1 if 0).
 */
void action_decrease_target_temp(lv_event_t *e) {
  intptr_t delta = (intptr_t)lv_event_get_user_data(e);
  int step = (delta != 0) ? (int)delta : 1;
  ui_bridge_change_target_temp(-step);
}

/**
 * @brief Toggles heater ON/OFF state or cancels active reflow stage.
 * @param e LVGL event object.
 */
void action_toggle_heating(lv_event_t *e) {
  (void)e;
  ui_bridge_toggle_heating();
}

/**
 * @brief Cycles to the next reflow profile (MANUAL -> LEAD FREE -> LEADED -> LOW TEMP).
 * @param e LVGL event object.
 */
void action_cycle_profile(lv_event_t *e) {
  (void)e;
  ui_bridge_cycle_profile();
}

} // extern "C"
