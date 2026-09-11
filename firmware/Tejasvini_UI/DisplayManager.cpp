#include "DisplayManager.h"
#include "UiConfig.h"
#include <PicoDVI.h>
#include <lvgl.h>

extern "C" {
#include "ui/ui.h"
#include "ui/screens.h"
}

static DVIGFX16 display(DVI_RES_400x240p60, picodvi_dvi_cfg, VREG_VOLTAGE_1_25);
static lv_color_t lvgl_buf[SCREEN_WIDTH * LVGL_BUF_LINES];
static lv_disp_draw_buf_t draw_buf;
static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;

static TouchManager* s_touch_instance = nullptr;

static void dvi_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    display.drawRGBBitmap(area->x1, area->y1, (const uint16_t *)color_p, w, h);
    lv_disp_flush_ready(disp_drv);
}

static void touch_read_cb(lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {
    (void)indev_drv;
    if (s_touch_instance && s_touch_instance->hasSignal()) {
        if (s_touch_instance->isTouched()) {
            data->state = LV_INDEV_STATE_PR;
            data->point.x = s_touch_instance->getLastX();
            data->point.y = s_touch_instance->getLastY();
        } else if (s_touch_instance->isReleased()) {
            data->state = LV_INDEV_STATE_REL;
        }
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

DisplayManager::DisplayManager(TouchManager& touch) : touch_(touch) {
    s_touch_instance = &touch_;
}

void DisplayManager::init() {
    // 1. Setup backlight (Active LOW)
    pinMode(PIN_BACKLIGHT, OUTPUT);
    digitalWrite(PIN_BACKLIGHT, LOW);

    // 2. Initialize PicoDVI
    display.begin();
    display.setRotation(DISPLAY_ROTATION);

    // 3. Initialize LVGL core
    lv_init();

    // 4. Register Display Buffer & Driver (240x400)
    lv_disp_draw_buf_init(&draw_buf, lvgl_buf, NULL, SCREEN_WIDTH * LVGL_BUF_LINES);
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCREEN_WIDTH;
    disp_drv.ver_res = SCREEN_HEIGHT;
    disp_drv.flush_cb = dvi_flush_cb;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // 5. Register Touch Controller
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touch_read_cb;
    lv_indev_drv_register(&indev_drv);

    // 6. Initialize EEZ Studio UI
    ui_init();

    // Offset dropdown list by -20px horizontally for optimal alignment
    if (objects.obj12) {
        lv_obj_t *list = lv_dropdown_get_list(objects.obj12);
        if (list) {
            lv_obj_set_style_translate_x(list, -20, LV_PART_MAIN);
            lv_obj_set_style_translate_x(list, -20, (lv_style_selector_t)LV_PART_MAIN | LV_STATE_SCROLLED);
            lv_obj_set_style_translate_y(list, -16, LV_PART_MAIN);
            lv_obj_set_style_translate_y(list, -16, (lv_style_selector_t)LV_PART_MAIN | LV_STATE_SCROLLED);
        }
    }
}

void DisplayManager::update() {
#if defined(LV_TICK_CUSTOM) && (LV_TICK_CUSTOM == 0)
    static uint32_t last_tick = 0;
    uint32_t now = millis();
    if (last_tick == 0) {
        last_tick = now;
    }
    lv_tick_inc(now - last_tick);
    last_tick = now;
#endif

    ui_tick();
    lv_timer_handler();
}
