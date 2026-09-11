#include "styles.h"
#include "images.h"
#include "fonts.h"

#include "ui.h"
#include "screens.h"

//
// Style: Text XS
//

void init_style_text_xs_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &lv_font_montserrat_8);
};

lv_style_t *get_style_text_xs_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_text_xs_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_text_xs(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_text_xs_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_text_xs(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_text_xs_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Text SM
//

void init_style_text_sm_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &lv_font_montserrat_10);
};

lv_style_t *get_style_text_sm_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_text_sm_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_text_sm(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_text_sm_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_text_sm(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_text_sm_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Text MD
//

void init_style_text_md_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &lv_font_montserrat_12);
};

lv_style_t *get_style_text_md_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_text_md_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_text_md(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_text_md_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_text_md(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_text_md_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Text LG
//

void init_style_text_lg_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &lv_font_montserrat_14);
};

lv_style_t *get_style_text_lg_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_text_lg_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_text_lg(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_text_lg_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_text_lg(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_text_lg_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Text XL
//

void init_style_text_xl_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &lv_font_montserrat_18);
};

lv_style_t *get_style_text_xl_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_text_xl_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_text_xl(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_text_xl_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_text_xl(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_text_xl_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Main Container
//

void init_style_main_container_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_COLUMN);
    lv_style_set_pad_row(style, 0);
    lv_style_set_pad_column(style, 0);
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_main_container_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_main_container_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_main_container(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_main_container_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_main_container(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_main_container_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Content
//

void init_style_content_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_flex_grow(style, 1);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_COLUMN);
    lv_style_set_pad_top(style, 10);
    lv_style_set_pad_bottom(style, 10);
    lv_style_set_pad_left(style, 10);
    lv_style_set_pad_right(style, 10);
};

lv_style_t *get_style_content_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_content_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_content(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_content_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_content(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_content_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Button Primary
//

void init_style_button_primary_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][4]));
    lv_style_set_radius(style, 5);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_border_side(style, LV_BORDER_SIDE_FULL);
    lv_style_set_clip_corner(style, true);
};

lv_style_t *get_style_button_primary_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_primary_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_button_primary_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_radius(style, 5);
    lv_style_set_clip_corner(style, true);
};

lv_style_t *get_style_button_primary_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_primary_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_button_primary(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_primary_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_button_primary_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_button_primary(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_primary_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_button_primary_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: Button Secondary
//

void init_style_button_secondary_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_radius(style, 5);
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][1]));
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_border_opa(style, 255);
    lv_style_set_border_width(style, 1);
    lv_style_set_border_side(style, LV_BORDER_SIDE_FULL);
    lv_style_set_flex_grow(style, 1);
    lv_style_set_clip_corner(style, true);
};

lv_style_t *get_style_button_secondary_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_secondary_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_button_secondary_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 100);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_border_opa(style, 255);
    lv_style_set_border_width(style, 1);
    lv_style_set_border_side(style, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(style, 5);
    lv_style_set_clip_corner(style, true);
};

lv_style_t *get_style_button_secondary_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_secondary_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_button_secondary(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_secondary_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_button_secondary_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_button_secondary(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_secondary_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_button_secondary_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: Seperator
//

void init_style_seperator_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_line_width(style, 1);
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
};

lv_style_t *get_style_seperator_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_seperator_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_seperator(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_seperator_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_seperator(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_seperator_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Panel
//

void init_style_panel_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_border_side(style, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(style, 5);
};

lv_style_t *get_style_panel_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_panel_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_panel(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_panel_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_panel(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_panel_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
//
//

void add_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*AddStyleFunc)(lv_obj_t *obj);
    static const AddStyleFunc add_style_funcs[] = {
        add_style_text_xs,
        add_style_text_sm,
        add_style_text_md,
        add_style_text_lg,
        add_style_text_xl,
        add_style_main_container,
        add_style_content,
        add_style_button_primary,
        add_style_button_secondary,
        add_style_seperator,
        add_style_panel,
    };
    add_style_funcs[styleIndex](obj);
}

void remove_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*RemoveStyleFunc)(lv_obj_t *obj);
    static const RemoveStyleFunc remove_style_funcs[] = {
        remove_style_text_xs,
        remove_style_text_sm,
        remove_style_text_md,
        remove_style_text_lg,
        remove_style_text_xl,
        remove_style_main_container,
        remove_style_content,
        remove_style_button_primary,
        remove_style_button_secondary,
        remove_style_seperator,
        remove_style_panel,
    };
    remove_style_funcs[styleIndex](obj);
}