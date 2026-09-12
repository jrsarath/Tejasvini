#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_set_target_temp(lv_event_t * e);
extern void action_increase_target_temp(lv_event_t * e);
extern void action_decrease_target_temp(lv_event_t * e);
extern void action_toggle_heating(lv_event_t * e);
extern void action_cycle_profile(lv_event_t * e);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/