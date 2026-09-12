#ifndef EEZ_LVGL_UI_STYLES_H
#define EEZ_LVGL_UI_STYLES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Style: Text XS
lv_style_t *get_style_text_xs_MAIN_DEFAULT();
void add_style_text_xs(lv_obj_t *obj);
void remove_style_text_xs(lv_obj_t *obj);

// Style: Text SM
lv_style_t *get_style_text_sm_MAIN_DEFAULT();
void add_style_text_sm(lv_obj_t *obj);
void remove_style_text_sm(lv_obj_t *obj);

// Style: Text MD
lv_style_t *get_style_text_md_MAIN_DEFAULT();
void add_style_text_md(lv_obj_t *obj);
void remove_style_text_md(lv_obj_t *obj);

// Style: Text LG
lv_style_t *get_style_text_lg_MAIN_DEFAULT();
void add_style_text_lg(lv_obj_t *obj);
void remove_style_text_lg(lv_obj_t *obj);

// Style: Text XL
lv_style_t *get_style_text_xl_MAIN_DEFAULT();
void add_style_text_xl(lv_obj_t *obj);
void remove_style_text_xl(lv_obj_t *obj);

// Style: Main Container
lv_style_t *get_style_main_container_MAIN_DEFAULT();
void add_style_main_container(lv_obj_t *obj);
void remove_style_main_container(lv_obj_t *obj);

// Style: Content
lv_style_t *get_style_content_MAIN_DEFAULT();
void add_style_content(lv_obj_t *obj);
void remove_style_content(lv_obj_t *obj);

// Style: Button Primary
lv_style_t *get_style_button_primary_MAIN_DEFAULT();
lv_style_t *get_style_button_primary_MAIN_PRESSED();
void add_style_button_primary(lv_obj_t *obj);
void remove_style_button_primary(lv_obj_t *obj);

// Style: Button Secondary
lv_style_t *get_style_button_secondary_MAIN_DEFAULT();
lv_style_t *get_style_button_secondary_MAIN_PRESSED();
void add_style_button_secondary(lv_obj_t *obj);
void remove_style_button_secondary(lv_obj_t *obj);

// Style: Seperator
lv_style_t *get_style_seperator_MAIN_DEFAULT();
void add_style_seperator(lv_obj_t *obj);
void remove_style_seperator(lv_obj_t *obj);

// Style: Panel
lv_style_t *get_style_panel_MAIN_DEFAULT();
void add_style_panel(lv_obj_t *obj);
void remove_style_panel(lv_obj_t *obj);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/