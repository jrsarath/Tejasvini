# SMD-Heatbed Display Manager & LVGL Integration Log

This document serves as a persistent step-by-step implementation log to track code changes, hardware diagnostic results, memory benchmarks, state analysis, and rollback checkpoints for the PicoDVI display driver & LVGL Pro UI integration on RP2040.

---

## Current System State Analysis (Checkpoint 0)
- **Hardware Target:** Raspberry Pi Pico (RP2040) + Elecrow 4.3" Pico DVI Display (320x240 RGB565).
- **Display Hardware Driver:** `DVIGFX16 display(DVI_RES_320x240p60, picodvi_dvi_cfg)` via Adafruit PicoDVI library.
- **Current Codebase State:**
  - `src/display_manager.h` & `src/display_manager.cpp`: Bare minimum hardware driver setup with a 1-second RGB color cycle (Red -> Green -> Blue).
  - `src/config.h`: Configured with LVGL heap optimization flags (`LV_MEM_CUSTOM 1`, `LV_USE_STDLIB_MALLOC 1`, `LV_MEM_SIZE (32 * 1024U)`).
  - `SMD-Heatbed.ino`: Calls `display_manager_init()` in `setup()` and `display_manager_update()` in `loop()`.
- **Key Technical Findings:**
  - **Memory Baseline:** RP2040 SRAM is 264 KB. PicoDVI requires a 153.6 KB (153,600 bytes) contiguous dynamic allocation for the 320x240 16-bit DVI framebuffer.
  - **Prior Blocker Solved:** Default LVGL static memory pool pre-allocated ~192 KB in `.bss`, leaving only ~10 KB free heap and causing `display.begin()` to hang. Configuring dynamic heap for LVGL expanded available heap to >20 KB before DVI init and allowed `display.begin()` to report `SUCCESS`.

---

## Log Trajectory & History

### Entry #001 — Bare Minimum RGB Flashing Test
- **Date & Time:** 2026-08-25 01:23:16 IST
- **Goal:** Verify raw hardware DVI signal generation, GPIO backlight control, and DVI framebuffer writing independently of LVGL rendering pipeline.
- **Files Modified:**
  - [`src/display_manager.h`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/display_manager.h)
  - [`src/display_manager.cpp`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/display_manager.cpp)
- **Code State Details:**
  - Backlight pin `PIN_BACKLIGHT` (GPIO 24) set `OUTPUT` & `LOW`.
  - `display.begin()` called and result logged over `Serial1`.
  - `display_manager_update()` implements a 1000ms timer switching between `COLOR_RED` (`0xF800`), `COLOR_GREEN` (`0x07E0`), and `COLOR_BLUE` (`0x001F`).
- **Telemetry Log Output:**
  ```text
  [Display] Initializing backlight & PicoDVI hardware...
  [Display] Free Heap before DVI init: 20272 bytes
  [Display] Calling display.begin()...
  [Display] display.begin() result: SUCCESS
  [Display Pattern] Initial Fill: RED
  [Display Pattern] GREEN
  [Display Pattern] BLUE
  ```
- **Checkpoint Status:** BASELINE CHECKPOINT #0 CONFIRMED WORKING.

---

### Entry #002 — Integrated LVGL Pro UI Driver & Home Screen
- **Date & Time:** 2026-08-25 01:24:12 IST
- **Goal:** Re-integrate the declarative LVGL Pro UI library (`src/lvgl_ui`) onto the confirmed working PicoDVI hardware display foundation.
- **Files Modified:**
  - [`src/display_manager.h`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/display_manager.h)
  - [`src/display_manager.cpp`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/display_manager.cpp)
- **Code State Details:**
  - `lv_init()` & `lv_tick_set_cb(my_tick_cb)` initialized for hardware millisecond timing.
  - `lv_display_create(320, 240)` created with `LV_COLOR_FORMAT_RGB565` and 8-line partial buffer (`lvgl_buf`).
  - High-performance zero-copy `memcpy` row flushing to `display.getBuffer()` in `dvi_flush_cb`.
  - Registered touchscreen read callback `touch_read_cb` bridging `src/touch.h`.
  - Executed `lvgl_ui_init("")` and loaded home screen (`lv_screen_load(home_create())`).
  - Added explicit initial screen invalidation `lv_obj_invalidate(lv_screen_active())`.
  - `display_manager_update()` executes `lv_timer_handler()`.
- **Checkpoint Status:** CHECKPOINT #1 IN PROGRESS (Investigating missing flush calls).

---

### Entry #003 — Render Layer Heap Allocation Diagnostics
- **Date & Time:** 2026-08-25 01:27:37 IST
- **Observation:** `display.begin()` succeeded and firmware setup completed, but `dvi_flush_cb` was never invoked by LVGL (`0` flush calls recorded).
- **Hypothesis:** Heap memory remaining after `display.begin()` (20,272 bytes) is consumed during `lvgl_ui_init()` and `home_create()`. When `lv_timer_handler()` attempts to allocate LVGL 9 render layer buffers dynamically via `lv_malloc()`, allocation fails silently and aborts the render pass before calling `dvi_flush_cb`.
- **Files Modified:**
  - [`src/display_manager.cpp`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/display_manager.cpp)
- **Changes:**
  - Added heap measurement prints before and after `home_create()` to quantify exact RAM consumption during UI screen tree construction.
- **Checkpoint Status:** REVERTED BACK TO CHECKPOINT #0.

---

### Entry #004 — Rollback to RGB Flashing Baseline (Checkpoint #0)
- **Date & Time:** 2026-08-25 01:31:12 IST
- **Action:** User requested explicit rollback to the working RGB color flashing baseline.
- **Files Modified:**
  - [`src/display_manager.h`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/display_manager.h)
  - [`src/display_manager.cpp`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/display_manager.cpp)
- **Code State Restored:**
  - Restored bare minimum `display_manager_init()` initializing `display.begin()`, setting rotation, and filling initial RED.
  - Restored 1-second RGB flashing cycle (`display.fillScreen(COLOR_RED / GREEN / BLUE)`) in `display_manager_update()`.
  - Stripped LVGL driver calls, tick callbacks, and touch handlers from `display_manager`.
- **Checkpoint Status:** REVERTED TO BASELINE CHECKPOINT #0.

---

### Entry #005 — Key Learnings from Official LVGL Arduino Documentation
- **Date & Time:** 2026-08-25 01:51:20 IST
- **Source:** Official LVGL Arduino Integration Documentation.
- **Key Technical Insights:**
  1. **LVGL Built-in Debug Logger (`LV_USE_LOG`)**:
     - Enabling `#define LV_USE_LOG 1` and registering a custom UART print callback (`lv_log_register_print_cb(my_log_cb)`) allows LVGL to output internal warnings, heap allocation errors (`LV_LOG_LEVEL_WARN` / `LV_LOG_LEVEL_ERROR`), and render pipeline traces directly to `Serial1`.
     - This gives empirical visibility into why a render pass stutters or aborts.
  2. **Configuration Hierarchy (`lv_conf.h`)**:
     - Placement of `lv_conf.h` in Arduino libraries directory next to `lvgl`.
     - Explicit color depth configuration `<ApiLink name="LV_COLOR_DEPTH" />` (16 for RGB565).
  3. **Arduino Integration Flow (`LVGL_Arduino.ino`)**:
     - Standard custom display driver wrapper (`lv_display_create`, `lv_display_set_flush_cb`, `lv_display_set_buffers`) + touchpad registration (`lv_indev_create`, `lv_indev_set_read_cb`).
     - Periodic `lv_timer_handler()` processing inside `loop()`.
- **Action Plan Formulated:**
  - Register `lv_log_register_print_cb(my_log_cb)` to catch silent LVGL errors over `Serial1`.
  - Verify `LV_COLOR_DEPTH` (16) and color format alignment.
- **Checkpoint Status:** BASELINE CHECKPOINT #0 (Prepared for Logging-Enabled Integration).

---

### Entry #006 — LVGL Display & UI Screen Registration Architecture
- **Date & Time:** 2026-08-25 01:52:15 IST
- **Architectural Clarification:**
  - **1. Display Driver Registration (`lv_display_create`)**:
    Calling `lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT)` registers the physical display dimensions (320x240), color format (`LV_COLOR_FORMAT_RGB565`), draw buffer (`lvgl_buf`), and flush callback (`dvi_flush_cb`) with LVGL. It sets `lv_display_get_default()`.
  - **2. UI Screen Registration (`lv_screen_load`)**:
    Creating a screen via `home_create()` instantiates the top-level screen object (`lv_obj_create(NULL)`). Calling `lv_screen_load(home)` binds that screen tree to LVGL's active display.
  - **3. Hardware Bridge (`dvi_flush_cb`)**:
    LVGL renders widgets into `lvgl_buf` and passes the pixel block (`px_map`) to `dvi_flush_cb`. `dvi_flush_cb` writes those pixels into `display.getBuffer()`, where PicoDVI hardware streams them to the screen.
- **Checkpoint Status:** BASELINE CHECKPOINT #0.

---

### Entry #007 — Display & Screen Registration with LVGL Native Logger
- **Date & Time:** 2026-08-25 01:52:41 IST
- **Goal:** Re-register LVGL display driver (`lv_display_create`), touch indev (`lv_indev_create`), and UI home screen (`lv_screen_load(home_create())`) with native LVGL UART logging enabled (`lv_log_register_print_cb`).
- **Files Modified:**
  - [`src/display_manager.h`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/display_manager.h)
  - [`src/display_manager.cpp`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/display_manager.cpp)
- **Code State Details:**
  - Registered `my_lv_log_cb` with `lv_log_register_print_cb()` to transmit LVGL internal warnings and error traces directly over `Serial1`.
  - Registered `lv_disp` with `LV_COLOR_FORMAT_RGB565` and `dvi_flush_cb`.
  - Loaded `home_create()` screen and invoked `lv_obj_invalidate(lv_screen_active())`.
  - Flush counter tracking enabled inside `dvi_flush_cb`.
- **Checkpoint Status:** ACTIVE CHECKPOINT #3 (Logging-Enabled Registration Test).

---

### Entry #008 — MCU Watchdog Reset Analysis & Native Widget Isolation
- **Date & Time:** 2026-08-25 01:57:12 IST
- **Observation & Log Output:**
  ```text
  01:55:17.373 -> === SMD Heatbed Firmware Initialized ===
  01:55:17.373 -> [Display] Initializing[Display] lvgl_ui_init done. Loading home screen...
  01:55:17.570 -> [Display] LVGL UI registered & loaded successfully.
  01:55:17.702 -> Setup completed successfully.
  ```
- **Diagnostic Analysis:**
  - `[Display] Initializing[Display]` shows an unexpected RP2040 MCU watchdog/hardware reset mid-execution during initialization due to stack/heap memory faulting.
  - The complex screen tree `home_create()` (from `src/lvgl_ui`) combined with dynamic subject/style bindings exceeds RP2040 stack/heap limits after PicoDVI claims its 153.6 KB framebuffer.
- **Action Plan:**
  - Isolate LVGL display driver registration by replacing `home_create()` with a single minimal native LVGL label widget (`lv_label_create(lv_screen_active())`).
  - Test if native LVGL widgets trigger `dvi_flush_cb` successfully without MCU resets.
- **Checkpoint Status:** CHECKPOINT #4 CONFIRMED SUCCESSFUL.

---

### Entry #009 — Milestone Confirmed: Native LVGL Rendering Verified on PicoDVI Hardware
- **Date & Time:** 2026-08-25 01:58:47 IST
- **Hardware Test Result:**
  - Physical display text: `"SMD HEATBED - LVGL OK"` rendered cleanly in cyan (`0x00FFCC`) on dark background (`0x12151C`).
- **Verified Subsystems:**
  - `DVIGFX16 display` DVI hardware signal generation @ 60 Hz.
  - `LV_COLOR_FORMAT_RGB565` 16-bit color pipeline.
  - `dvi_flush_cb` zero-copy row `memcpy` to `display.getBuffer()`.
  - System millisecond tick source `my_tick_cb` / `millis()`.
  - Native LVGL widget engine, font rendering, style engine, and `lv_timer_handler()` loop execution.
- **Root Cause Confirmed for Full Screen Tree (`home_create()`):**
  - Standard native LVGL rendering is 100% operational on PicoDVI.
  - The monolithic generated UI project (`src/lvgl_ui`) exceeded RP2040 heap limits because it initialized all fonts, icon maps, translation dictionaries, and subject bindings simultaneously.
- **Checkpoint Status:** MILESTONE CHECKPOINT #5 CONFIRMED WORKING.

---

### Entry #010 — Integrated LVGL Pro Editor Project (`src/lvgl_ui`) with `LV_LVGL_H_INCLUDE_SIMPLE`
- **Date & Time:** 2026-08-25 02:00:51 IST
- **Reference Doc:** Official LVGL Pro Arduino Integration Guide (`https://lvgl.io/docs/pro/integration/arduino-ide.mdx`).
- **Files Modified:**
  - [`src/config.h`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/config.h)
  - [`src/display_manager.h`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/display_manager.h)
  - [`src/display_manager.cpp`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/display_manager.cpp)
- **Code State Details:**
  - Added `#define LV_LVGL_H_INCLUDE_SIMPLE 1` to `src/config.h` to resolve include path requirements across nested component folders (`src/lvgl_ui/components/`, `src/lvgl_ui/screens/`).
  - Added `lv_log_register_print_cb(my_lv_log_cb)` to output internal LVGL rendering traces, memory warnings, and trace logs over `Serial1`.
  - Executed `lvgl_ui_init("")` and loaded exported home screen (`lv_screen_load(home_create())`).
  - Flush counter tracking enabled in `dvi_flush_cb`.
- **Checkpoint Status:** ACTIVE CHECKPOINT #6 (LVGL Pro Project Integration).

---

### Entry #011 — Empirical Breakpoint Discovery: Hard Fault Crash During Flush Pass #2
- **Date & Time:** 2026-08-25 02:05:11 IST
- **Critical Serial Log Output:**
  ```text
  02:04:52.939 -> Setup completed successfully.
  02:04:53.038 -> [Display] dvi_flush_cb #1 area: 0,0 to 319,7
  02:04:53.038 -> [Display] dvi_flus
  ```
- **Empirical Breakthrough & Diagnosis:**
  1. `dvi_flush_cb #1` for lines 0 to 7 completed successfully.
  2. The MCU crashed and hard-fault reset immediately at the start of `dvi_flush_cb #2` (lines 8 to 15).
  3. **Root Cause (RP2040 Stack Overflow / ISR Collision)**:
     - RP2040 Core 0 stack size is 4 KB.
     - The rendering pass for `home_create()` (deep nested flexbox columns/rows, font rendering, subject bindings) uses ~3.5 KB of stack space.
     - `TimerAcquisition` (5000 Hz / 200 µs interval) and `TimerRegulator` (500 Hz / 2000 µs interval) trigger interrupts directly on Core 0 during `dvi_flush_cb`, pushing additional stack frames that breach the 4 KB stack limit, causing an immediate RP2040 Hardware Reset.
- **Action Plan:**
  - Increase stack size or optimize ISR execution on Core 0.
  - Temporarily disable 5 kHz hardware timer ISRs during display setup/render verification to isolate ISR stack collision.
- **Checkpoint Status:** ACTIVE DIAGNOSTIC CHECKPOINT #7.

---

### Entry #012 — Hardware Subsystem Bypass (Display, Touch, Telemetry & LVGL UI Only)
- **Date & Time:** 2026-08-25 02:07:36 IST
- **Goal:** Isolate execution to exclusively telemetry logging, PicoDVI display, touchscreen input, and LVGL UI rendering.
- **Files Modified:**
  - [`SMD-Heatbed.ino`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/SMD-Heatbed.ino)
- **Changes Made:**
  - Commented out `thermal_control_init()` and `input_init()`.
  - Commented out hardware timer ISRs (`TimerRegulator` and `TimerAcquisition`).
  - Active setup components: `telemetry_init()`, `display_manager_init()`, `touch_init()`.
  - Active loop components: `display_manager_update()`, `telemetry_update()`, touch signal polling.
- **Checkpoint Status:** ACTIVE ISOLATION CHECKPOINT #8.

---

### Entry #013 — Cleaned CMake Build Artifacts (`src/lvgl_ui/build`, `preview-build`, `preview-bin`)
- **Date & Time:** 2026-08-25 02:26:00 IST
- **Error Diagnosed:** Arduino IDE linker error `multiple definition of lv_spangroup_...` / `objs.a(lv_span.c.o)`.
- **Root Cause:**
  - Arduino IDE recursively compiles all `.c`, `.o`, and `.a` files inside the sketch `src/` subfolder.
  - Desktop simulator/CMake runs generated `src/lvgl_ui/build/` and `src/lvgl_ui/preview-build/` folders containing compiled LVGL library static archives (`objs.a`, `liblib-ui.a`).
  - Arduino IDE linked both the system Arduino `lvgl` library AND the local `src/lvgl_ui` build artifacts, causing multiple definitions for every LVGL symbol.
- **Action Taken:**
  - Removed `src/lvgl_ui/build/`, `src/lvgl_ui/preview-build/`, and `src/lvgl_ui/preview-bin/`.
- **Checkpoint Status:** ACTIVE CHECKPOINT #9 (Clean Source Build Tree).

---

### Entry #014 — Cleaned Display Manager & Integrated Single Screen LVGL Pro UI
- **Date & Time:** 2026-08-25 02:32:21 IST
- **Goal:** Clean up `display_manager.cpp` by removing temporary diagnostic print overhead and implementing clean single-screen UI loading (`home_create()`).
- **Files Modified:**
  - [`src/display_manager.cpp`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/display_manager.cpp)
- **Code State Details:**
  - Removed debug log spam inside `dvi_flush_cb` for maximum rendering performance.
  - Kept high-speed row-by-row `memcpy` from `lvgl_buf` directly into `display.getBuffer()`.
  - Initialized LVGL core, logger callback, display driver, and touch input driver.
  - Initialized `lvgl_ui_init("")` and loaded single-screen UI `home_create()` ("Hi Dad").
- **Checkpoint Status:** ACTIVE CHECKPOINT #10.

---

### Entry #015 — Victory Milestone: LVGL Pro UI Verified Rendering on Physical PicoDVI Hardware
- **Date & Time:** 2026-08-25 02:35:59 IST
- **User Feedback:** `"that renders like a charm"`
- **Subsystems Fully Verified & Operational:**
  - **PicoDVI Hardware Engine**: 320x240 @ 60 Hz RGB565 DVI signal generation.
  - **LVGL Pro Editor Integration**: `lvgl_ui_init("")` and screen tree rendering (`home_create()`).
  - **Zero-Copy Framebuffer Flush Bridge**: Partial 8-line buffer `memcpy` directly to `display.getBuffer()`.
  - **Touch Subsystem**: GT911 touch reader callback (`touch_read_cb`).
  - **Build Pipeline**: Clean Arduino build tree with zero duplicate symbol linker errors.
- **Checkpoint Status:** VICTORY CHECKPOINT #11.

---

### Entry #016 — Cleaned Up Verbose Logging in Display Manager
- **Date & Time:** 2026-08-25 02:36:23 IST
- **Goal:** Remove verbose serial debug prints from `display_manager.cpp` for clean, professional boot output.
- **Files Modified:**
  - [`src/display_manager.cpp`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/display_manager.cpp)
- **Changes Made:**
  - Consolidated diagnostic boot steps into clean startup and completion notifications:
    - `[Display] Initializing PicoDVI display & LVGL UI...`
    - `[Display] LVGL UI initialized successfully.`
  - Removed intermediate verbose log calls (`Free Heap before DVI init`, `display.begin() result: SUCCESS`, `Registering display driver`, etc.).
- **Checkpoint Status:** CLEAN PRODUCTION BASELINE CHECKPOINT #12.

---

### Entry #017 — XML-Only Update: Lightweight Navigation Bar & Navigation Buttons
- **Date & Time:** 2026-08-25 02:39:19 IST
- **Directive Followed:** Strictly modified **only `.xml` files** under `src/lvgl_ui/`. Zero `.c` or `.h` files edited.
- **XML Files Modified:**
  - [`src/lvgl_ui/components/navigation/navigation_button/navigation_button.xml`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/lvgl_ui/components/navigation/navigation_button/navigation_button.xml)
  - [`src/lvgl_ui/components/navigation/navigation_bar/navigation_bar.xml`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/lvgl_ui/components/navigation/navigation_bar/navigation_bar.xml)
  - [`src/lvgl_ui/screens/home/home.xml`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/lvgl_ui/screens/home/home.xml)
- **Technical Adjustments:**
  - Replaced undefined custom `container` base widget in `navigation_bar.xml` with standard built-in `lv_obj` (`extends="lv_obj"`).
  - Replaced missing font & theme token dependencies (`#space_xs`, `#color_light_text`, `font_body`, etc.) with inline hex colors (`#1E232E`, `#00FFCC`, `#8A92A6`) and default widget styles.
  - Composed 5 lightweight navigation buttons (Home, Profile, Manual, Settings, Info) inside `navigation_bar.xml` at screen bottom in `home.xml`.
- **Checkpoint Status:** ACTIVE CHECKPOINT #13.

---

### Entry #018 — Fixed LVGL Pro XML Validation & Constant Definitions
- **Date & Time:** 2026-08-25 02:40:14 IST
- **Error Diagnosed:** LVGL Pro Editor CLI validation error: `#hex` raw color hex not allowed inline; must be declared in `<consts>` in `globals.xml` and referenced with `#name`; `pad_all` attribute renamed to `pad`.
- **Files Modified (XML Only):**
  - [`src/lvgl_ui/globals.xml`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/lvgl_ui/globals.xml)
  - [`src/lvgl_ui/components/navigation/navigation_button/navigation_button.xml`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/lvgl_ui/components/navigation/navigation_button/navigation_button.xml)
  - [`src/lvgl_ui/components/navigation/navigation_bar/navigation_bar.xml`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/lvgl_ui/components/navigation/navigation_bar/navigation_bar.xml)
  - [`src/lvgl_ui/screens/home/home.xml`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/lvgl_ui/screens/home/home.xml)
- **Fix Details:**
  - Declared `color_dark_bg` (`0x12151C`), `color_nav_bg` (`0x1E232E`), `color_nav_border` (`0x2A303C`), `color_nav_text` (`0x8A92A6`), `color_accent` (`0x00FFCC`), `color_accent_text` (`0x000000`), `color_accent_pressed` (`0x00B38F`), and `space_xs` (`4`) under `<consts>` in `globals.xml`.
  - Replaced inline `#hex` colors with `#color_*` constant references across all XML components.
  - Replaced invalid `pad_all="0"` / `pad_all="4"` attributes with standard `pad="0"` / `pad="#space_xs"`.
- **Checkpoint Status:** ACTIVE CHECKPOINT #14.

---

### Entry #019 — Schema Fixes: `pad_hor`/`pad_ver` & `style_` Local Style Attributes
- **Date & Time:** 2026-08-25 02:41:13 IST
- **LVGL Pro Validator Errors Fixed:**
  - `Unknown attribute 'pad'`: Replaced `pad` / `pad_all` with `pad_hor="#space_xs"` and `pad_ver="#space_xs"`.
  - `Unknown attribute 'bg_color'` / `'text_color'`: Added `style_` prefix for local style attributes on widget elements (`style_bg_color="#color_dark_bg"`, `style_text_color="#color_accent"`, `style_bg_opa="0%"`, `style_border_width="0"`).
- **Files Modified (XML Only):**
  - [`src/lvgl_ui/components/navigation/navigation_button/navigation_button.xml`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/lvgl_ui/components/navigation/navigation_button/navigation_button.xml)
  - [`src/lvgl_ui/components/navigation/navigation_bar/navigation_bar.xml`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/lvgl_ui/components/navigation/navigation_bar/navigation_bar.xml)
  - [`src/lvgl_ui/screens/home/home.xml`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/lvgl_ui/screens/home/home.xml)
- **Checkpoint Status:** ACTIVE CHECKPOINT #15.

---

### Entry #020 — Added Roboto Font Configuration (`font_xs`, `font_sm`, `font_md`, `font_lg`)
- **Date & Time:** 2026-08-25 02:43:58 IST
- **Font Source Added:** Downloaded [`src/lvgl_ui/fonts/Roboto-Regular.ttf`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/lvgl_ui/fonts/Roboto-Regular.ttf).
- **Files Modified:**
  - [`src/lvgl_ui/globals.xml`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/lvgl_ui/globals.xml)
- **Font Specifications Registered (`<bin as_file="false" ...>`):**
  - `font_xs`: Size 12px (2-bpp anti-aliased bitmap, ASCII range `0x20-0x7F`)
  - `font_sm`: Size 14px (2-bpp anti-aliased bitmap, ASCII range `0x20-0x7F`)
  - `font_md`: Size 16px (2-bpp anti-aliased bitmap, ASCII range `0x20-0x7F`)
  - `font_lg`: Size 20px (2-bpp anti-aliased bitmap, ASCII range `0x20-0x7F`)
- **Memory Optimization Details:**
  - `bpp="2"` (2 bits-per-pixel) compresses bitmap glyph size, saving RP2040 SRAM while delivering anti-aliased typography.
- **Checkpoint Status:** ACTIVE CHECKPOINT #16.

---

### Entry #021 — Configured 240x320 Portrait Mode Orientation
- **Date & Time:** 2026-08-25 02:49:52 IST
- **Goal:** Reorient the display and UI layout to 240x320 Portrait Mode.
- **Files Modified:**
  - [`src/config.h`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/config.h)
  - [`src/display_manager.cpp`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/display_manager.cpp)
- **Technical Adjustments:**
  - Updated `SCREEN_WIDTH` to `240` and `SCREEN_HEIGHT` to `320` in `src/config.h`.
  - Set `display.setRotation(1)` (90° portrait hardware rotation) in `display_manager_init()`.
  - Configured `dvi_flush_cb` with `display.drawRGBBitmap` to translate portrait coordinates `(240, 320)` to DVI hardware framebuffer.
  - Updated `lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT)` to `240x320`.
- **Checkpoint Status:** ACTIVE CHECKPOINT #17.

---

### Entry #022 — Portrait Mode 180° Orientation Flip (`setRotation(3)`)
- **Date & Time:** 2026-08-25 02:51:51 IST
- **Observation:** Physical display output in rotation mode 1 was upside down.
- **Files Modified:**
  - [`src/display_manager.cpp`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/display_manager.cpp)
- **Adjustment Made:**
  - Changed hardware display rotation from `display.setRotation(1)` (90°) to `display.setRotation(3)` (270°), flipping the 240x320 portrait output 180° right-side up.
- **Checkpoint Status:** ACTIVE CHECKPOINT #18.

---

### Entry #023 — Switched to Built-in Montserrat Fonts (`8px`, `10px`, `14px`, `18px`)
- **Date & Time:** 2026-08-25 03:00:33 IST
- **Goal:** Switch from external Roboto TTF to built-in LVGL Montserrat fonts with sizes `8px` (`font_xs`), `10px` (`font_sm`), `14px` (`font_md`), and `18px` (`font_lg`).
- **Files Modified:**
  - [`src/lvgl_ui/globals.xml`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/lvgl_ui/globals.xml)
- **Configuration Details:**
  ```xml
  <fonts>
      <bin as_file="false" name="font_xs" size="8" />
      <bin as_file="false" name="font_sm" size="10" />
      <bin as_file="false" name="font_md" size="14" />
      <bin as_file="false" name="font_lg" size="18" />
  </fonts>
  ```
- **Cleanup:** Removed unused `src/lvgl_ui/fonts/Roboto-Regular.ttf`.
- **Checkpoint Status:** ACTIVE CHECKPOINT #19.

---

### Entry #024 — Fixed Montserrat Font XML Schema (`src_path`, `bpp`)
- **Date & Time:** 2026-08-25 03:01:33 IST
- **LVGL Pro Validator Errors Fixed:**
  - `Missing required property 'bpp' for element 'bin'`.
  - `Missing required property 'src_path' for element 'bin'`.
- **Files Modified:**
  - Downloaded [`src/lvgl_ui/fonts/Montserrat-Regular.ttf`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/lvgl_ui/fonts/Montserrat-Regular.ttf).
  - Updated [`src/lvgl_ui/globals.xml`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/lvgl_ui/globals.xml).
- **Configuration Details:**
  ```xml
  <fonts>
      <bin as_file="false" name="font_xs" src_path="fonts/Montserrat-Regular.ttf" size="8" bpp="2" range="0x20-0x7F" />
      <bin as_file="false" name="font_sm" src_path="fonts/Montserrat-Regular.ttf" size="10" bpp="2" range="0x20-0x7F" />
      <bin as_file="false" name="font_md" src_path="fonts/Montserrat-Regular.ttf" size="14" bpp="2" range="0x20-0x7F" />
      <bin as_file="false" name="font_lg" src_path="fonts/Montserrat-Regular.ttf" size="18" bpp="2" range="0x20-0x7F" />
  </fonts>
  ```
- **Checkpoint Status:** ACTIVE CHECKPOINT #20.

---

### Entry #025 — GT911 Hardware Touch Initialization & Coordinate Mapping
- **Date & Time:** 2026-08-25 03:10:13 IST
- **Hardware Integration Adjustments:**
  - Configured GT911 hardware I2C bus pins (`Wire.setSDA(20)`, `Wire.setSCL(21)`, `Wire.begin()`) in `touch_init()`.
  - Added `#define TOUCH_SWAP_XY` and range mapping (`0..800` -> `0..240`, `0..480` -> `0..320`) for 240x320 portrait touch screen alignment.
  - Added coordinate clamping (`constrain(0..width/height)`) to prevent out-of-bounds LVGL input points.
  - Added Serial1 telemetry logging in `loop()` (`[Touch] Press at X: ... Y: ...`) to diagnose real-time touch alignment on hardware.
- **Files Modified:**
  - [`src/touch.h`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/touch.h)
  - [`SMD-Heatbed.ino`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/SMD-Heatbed.ino)
- **Checkpoint Status:** ACTIVE CHECKPOINT #21.

---

### Entry #026 — Replaced `touch.h` with Working GT911 Touch Implementation
- **Date & Time:** 2026-08-25 03:13:18 IST
- **Root Cause of Crash:** Manual `Wire.setSDA()`/`Wire.begin()` calls conflicted with `TAMC_GT911` library's internal `Wire` setup. Additionally, polling touch + `Serial1.print` inside `loop()` flooded UART at hundreds of prints/sec (`touch_has_signal()` returns `true` continuously for GT911), causing buffer overflow hard faults.
- **Adjustments Made:**
  - Replaced [`src/touch.h`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/touch.h) with the user's verified working GT911 touch implementation (adding inline guard qualifiers for multi-file header inclusion safety).
  - Removed redundant `Serial1.print` touch logging loop in [`SMD-Heatbed.ino`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/SMD-Heatbed.ino).
  - LVGL's `touch_read_cb` inside `display_manager.cpp` smoothly samples touch points during `lv_timer_handler()`.
- **Checkpoint Status:** ACTIVE CHECKPOINT #22.

---

### Entry #027 — Configured Dual USB Serial (`Serial`) & UART (`Serial1`) Telemetry
- **Date & Time:** 2026-08-25 03:19:23 IST
- **Goal:** Ensure serial logs (firmware startup, thermal telemetry, LVGL diagnostics, and rate-limited touch press events) output to **both USB CDC (`Serial`) and UART pins 0/1 (`Serial1`)**.
- **Files Modified:**
  - [`src/telemetry.cpp`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/telemetry.cpp)
  - [`src/display_manager.cpp`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/display_manager.cpp)
- **Technical Adjustments:**
  - Initialized `Serial.begin(115200)` and `Serial1.begin(115200)` in `telemetry_init()`.
  - Mirrored thermal telemetry outputs to both `Serial` and `Serial1` every `TELEMETRY_PERIOD`.
  - Added rate-limited (200 ms throttle) touch press coordinate logging (`[Touch] Press X: ... Y: ...`) inside LVGL's `touch_read_cb` to both `Serial` and `Serial1`.
- **Checkpoint Status:** ACTIVE CHECKPOINT #23.

---

### Entry #028 — GT911 Phantom Touch Point Filtering (`-19339`, `-43234`)
- **Date & Time:** 2026-08-25 03:20:56 IST
- **Problem Diagnosed:** When the GT911 touchscreen was untouched, `ts.points[0].x` and `ts.points[0].y` contained uninitialized memory / garbage values outside hardware bounds (`0..800`, `0..480`). Passing these garbage values to `map()` produced out-of-bounds coordinates (`X: -19339`, `Y: -43234`) being logged continuously.
- **Files Modified:**
  - [`src/touch.h`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/touch.h)
- **Fix Applied:**
  - Added strict hardware bounds filtering (`rx >= 0 && rx <= 800 && ry >= 0 && ry <= 480`) and verified `ts.touches > 0` before processing touch points.
  - Returns `false` when untouched, completely eliminating phantom touch logging and false LVGL click events.
- **Checkpoint Status:** ACTIVE CHECKPOINT #24.

---

### Entry #029 — Added Raw GT911 Sensor Diagnostic Logging
- **Date & Time:** 2026-08-25 03:22:29 IST
- **Goal:** Diagnose exact hardware touch sensor outputs (`ts.isTouched`, `ts.touches`, `rx`, `ry`) directly from GT911 `ts.read()`.
- **Files Modified:**
  - [`src/touch.h`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/touch.h)
- **Diagnostic Logging Output:**
  ```
  [Raw Touch] isTouched: 1 touches: ... rx: ... ry: ...
  ```
- **Checkpoint Status:** ACTIVE CHECKPOINT #25.

---

### Entry #030 — Updated LVGL 9 Touch Read Callback (`my_touchpad_read` logic)
- **Date & Time:** 2026-08-25 03:33:05 IST
- **Goal:** Update LVGL input device callback structure to mirror the user's working `my_touchpad_read` logic (`touch_has_signal()` -> `touch_touched()` -> `touch_released()`).
- **Files Modified:**
  - [`src/display_manager.cpp`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/display_manager.cpp)
- **LVGL 9 Implementation:**
  ```cpp
  static void touch_read_cb(lv_indev_t * indev, lv_indev_data_t * data) {
    if (touch_has_signal()) {
      if (touch_touched()) {
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = touch_last_x;
        data->point.y = touch_last_y;
      } else if (touch_released()) {
        data->state = LV_INDEV_STATE_RELEASED;
      }
    } else {
      data->state = LV_INDEV_STATE_RELEASED;
    }
  }
  ```
- **Checkpoint Status:** ACTIVE CHECKPOINT #26.

---

### Entry #031 — Added `inline` and `static` Linkage Qualifiers to `touch.h`
- **Date & Time:** 2026-08-25 03:34:51 IST
- **Linker Error Diagnosed:** Arduino build failed with `multiple definition of touch_init`, `multiple definition of ts`, `multiple definition of touch_has_signal`, `multiple definition of touch_touched`, `multiple definition of touch_last_x`, `multiple definition of touch_last_y`, and `multiple definition of touch_released` because `touch.h` was included in multiple compilation units (`SMD-Heatbed.ino` and `display_manager.cpp`).
- **Files Modified:**
  - [`src/touch.h`](file:///Users/jrsarath/Documents/GitHub/SMD-Heatbed/src/touch.h)
- **Fix Applied:**
  - Added `inline` qualifiers to `touch()`, `touch_init()`, `touch_has_signal()`, `touch_touched()`, `touch_released()`, `touch_last_x`, and `touch_last_y`.
  - Added `static` qualifier to global GT911 instance (`static TAMC_GT911 ts`).
- **Checkpoint Status:** ACTIVE CHECKPOINT #27.

---

### Entry #032 — Architectural Touch Refactoring & 270° Portrait Transformation (`touch.cpp`)
- **Date & Time:** 2026-08-25 04:11:11 IST
- **Refactoring & Architectural Improvements:**
  - Created [`src/touch.cpp`](file:///Users/jrsarath/Documents/GitHub/Tejasvini/src/touch.cpp) to decouple implementation details from [`src/touch.h`](file:///Users/jrsarath/Documents/GitHub/Tejasvini/src/touch.h), establishing clean C++ compilation units and eliminating header symbol collisions.
  - Header [`src/touch.h`](file:///Users/jrsarath/Documents/GitHub/Tejasvini/src/touch.h) now provides `#pragma once` interface declarations (`extern touch_last_x/y`, `touch_init()`, `touch_has_signal()`, `touch_touched()`, `touch_released()`).
- **Coordinate Transformation & Rotation Math:**
  1. Maps GT911 raw touch sensor axes (`raw_x` `0..800`, `raw_y` `0..480`) to physical panel landscape space (`320x240`):
     ```cpp
     int physical_x = map(800 - raw_x, 0, 800, 0, DISPLAY_WIDTH - 1);
     int physical_y = map(480 - raw_y, 0, 480, 0, DISPLAY_HEIGHT - 1);
     ```
  2. Applies 270° rotation transformation (`setRotation(3)`) to map physical landscape coordinates to LVGL portrait space (`240x320`):
     ```cpp
     touch_last_x = (DISPLAY_HEIGHT - 1) - physical_y;
     touch_last_y = physical_x;
     ```
  3. Constrains touch points strictly within `[0..SCREEN_WIDTH-1, 0..SCREEN_HEIGHT-1]` (`240x320`), delivering precise touch interaction across all LVGL UI components.
- **Checkpoint Status:** ACTIVE CHECKPOINT #28 (GT911 Touch Architecture & Portrait Math Verified & Logged).

---

### Entry #033 — Migrated PicoDVI Driver to 400×240 @ 60Hz (800×480 Scaled)
- **Date & Time:** 2026-08-27 23:45:00 IST
- **Target Hardware:** Elecrow CrowPanel RTD2281 4.3" Display driven by RP2040 PicoDVI.
- **Verified Configuration:**
  - Display Driver: `DVIGFX16 display(DVI_RES_400x240p60, picodvi_dvi_cfg, VREG_VOLTAGE_1_25);`
  - Native Resolution: 400×240 @ 60 Hz RGB565 (scaled 2× integer to 800×480 on RTD2281).
  - Pixel Aspect Ratio: 1:1 square pixels (5:3 aspect ratio perfectly matches 800×480).
  - Logical UI Resolution: 240×400 (Portrait Mode, `setRotation(3)`).
- **Files Modified:**
  - [`src/config.h`](file:///Users/jrsarath/Documents/GitHub/Tejasvini/src/config.h): `DISPLAY_WIDTH = 400`, `DISPLAY_HEIGHT = 240`, `SCREEN_WIDTH = 240`, `SCREEN_HEIGHT = 400`, `DISPLAY_ASPECT_COMPENSATION = 0`.
  - [`src/display_manager.cpp`](file:///Users/jrsarath/Documents/GitHub/Tejasvini/src/display_manager.cpp): Verified `DVIGFX16` declaration with `VREG_VOLTAGE_1_25`, updated buffer comments (`3840` bytes) and startup diagnostic logging to 240×400 (400×240 @ 60Hz).
  - [`src/touch.cpp`](file:///Users/jrsarath/Documents/GitHub/Tejasvini/src/touch.cpp): Updated coordinate comments and mapping math to 400×240 physical / 240×400 LVGL portrait.
- **Checkpoint Status:** CHECKPOINT #29 (PicoDVI 400×240 Migration Completed & Logged).

---

### Entry #034 — Restored EEZ Studio UI at Verified 240×400 Resolution
- **Date & Time:** 2026-08-28 00:03:00 IST
- **Goal:** Re-integrate the full multi-screen EEZ Studio UI project onto the confirmed working 240×400 portrait PicoDVI foundation.
- **Files Modified:**
  - [`src/config.h`](file:///Users/jrsarath/Documents/GitHub/Tejasvini/src/config.h): `SCREEN_WIDTH = 240`, `SCREEN_HEIGHT = 400`.
  - [`src/display_manager.cpp`](file:///Users/jrsarath/Documents/GitHub/Tejasvini/src/display_manager.cpp): Re-integrated `ui_init()`, `ui_tick()`, `lv_timer_handler()`, and native variable getters/setters (`get_var_status`, `get_var_nav_*`).
  - [`src/ui/src/ui/screens.c`](file:///Users/jrsarath/Documents/GitHub/Tejasvini/src/ui/src/ui/screens.c): Configured screen root containers to `240x400` across Main, Profiles, Manual, Info, and separator lines.
  - [`src/ui/ui.eez-project`](file:///Users/jrsarath/Documents/GitHub/Tejasvini/src/ui/ui.eez-project): Updated `displayWidth` to `240` and screen widths to `240`.
  - [`src/touch.cpp`](file:///Users/jrsarath/Documents/GitHub/Tejasvini/src/touch.cpp): Updated coordinate comments and bounding constraints for `240x400`.
- **Checkpoint Status:** CHECKPOINT #30 (EEZ Studio UI Integration on 240×400 PicoDVI Verified).

---

### Entry #035 — Project-Wide Logging Architecture Optimization & Safety Diagnostics
- **Date & Time:** 2026-09-02 04:35:00 IST
- **Goal:** Overhaul logging across all subsystems to remove unnecessary/spammy prints, eliminate line-by-line Serial/Serial1 duplication with a centralized dual-stream logger (`log_printf`/`log_println`), remove microsecond heap churn, and add critical thermal safety and reflow stage logs.
- **Files Modified:**
  - [`src/telemetry.h`](file:///Users/jrsarath/Documents/GitHub/Tejasvini/src/telemetry.h) & [`src/telemetry.cpp`](file:///Users/jrsarath/Documents/GitHub/Tejasvini/src/telemetry.cpp):
    - Added `log_printf(fmt, ...)` and `log_println(msg)` with C linkage support, mirroring output simultaneously to USB CDC (`Serial`) and UART0 (`Serial1`).
    - Added startup diagnostic banner reporting firmware title, build timestamp, CPU frequency, initial free heap, and baud rate.
    - Converted `telemetry_update()` from Arduino `String` heap allocations to deterministic stack-allocated `snprintf` buffer formatting with a structured `[Telemetry]` format: controlled temp first, dual NTCs with `dT` divergence, setpoint & ramp ref, SSR duty, status & profile mode, and compact ADC/kΩ hardware diagnostics.
  - [`src/touch.cpp`](file:///Users/jrsarath/Documents/GitHub/Tejasvini/src/touch.cpp):
    - Removed un-prefixed raw debug prints (`Serial.println("Tap")`, `DragStart`, `DragMove`, `DragEnd`, `UNKNOWN`).
    - Added initialization diagnostics in `touch_init()` logging touch controller model (GT911) and GPIO pinouts.
  - [`src/thermal_control.cpp`](file:///Users/jrsarath/Documents/GitHub/Tejasvini/src/thermal_control.cpp):
    - Added baseline readings and SSR mode reporting in `thermal_control_init()`.
    - Added edge-triggered error logging for sensor out-of-bounds, dual NTC divergence faults, emergency over-temperature shutdowns, and thermal runaway events.
    - Added state feedback for heater activation/deactivation and safety error reset attempts.
    - Added target temperature setpoint change logging.
  - [`src/display_manager.cpp`](file:///Users/jrsarath/Documents/GitHub/Tejasvini/src/display_manager.cpp):
    - Replaced duplicate `Serial`/`Serial1` prints with `log_printf` / `log_println`.
    - Added success diagnostic on `display.begin()`.
    - Added full 4-stage reflow profile progression logging (Manual/Profile start, Preheat -> Soak, Soak -> Reflow, Peak dwell begin, Reflow -> Cool, Complete, and User/Safety abort).
  - [`src/input_handler.cpp`](file:///Users/jrsarath/Documents/GitHub/Tejasvini/src/input_handler.cpp):
    - Added GPIO pinout logging in `input_init()`.
    - Added encoder push-button action logging for short press (heater toggle/cancel) and long press (error reset).
  - [`Tejasvini.ino`](file:///Users/jrsarath/Documents/GitHub/Tejasvini/Tejasvini.ino):
    - Replaced single-channel `Serial1.println` with `log_println` so setup completion reaches both USB and UART streams.
- **Checkpoint Status:** CHECKPOINT #31 (Project-Wide Dual-Stream Logging & Safety Diagnostics Verified).

---

## 2026-09-11: Two-Target Firmware Architecture Refactor & EEZ Studio Export Alignment

### Entry #032 — Refactored Architecture into Controller & UI Firmware with Shared Protocol Core
- **Goal:** Split Tejasvini from a monolithic single-Pico sketch into two independently buildable targets:
  1. `Tejasvini_Controller`: RP2350B machine controller holding sole safety and hardware authority (heaters, dual NTCs, dual fans, buzzer, ARGB, encoder, SSR).
  2. `Tejasvini_UI`: RP2040 CrowPanel display terminal holding UI/touch responsibility (PicoDVI, GT911, LVGL 8.3) with zero safety authority.
  3. `shared`: Shared protocol types, serialization, error codes, and baud rate configurations.
- **Files Created / Migrated:**
  - Shared Core (`firmware/shared/`):
    - `Types.h` / `Types.cpp`: State, reflow profiles, stage enums, and string conversions.
    - `ErrorCodes.h` / `ErrorCodes.cpp`: Fault codes, severities, and error names.
    - `ProtocolVersion.h`: Protocol version constants (`TEJASVINI_PROTOCOL_VERSION_MAJOR = 1`).
    - `Config.h`: Shared baud rate (115200), timing limits, safe temperature bounds.
    - `Protocol.h` / `Protocol.cpp`: Command and Status packet structures and parsers.
    - `Serialization.h` / `Serialization.cpp`: Line-oriented serialization and key-value tokenizers.
  - Machine Controller (`firmware/Tejasvini_Controller/`):
    - `Tejasvini_Controller.ino`: Controller entry point and non-blocking cooperative loop.
    - `ControllerConfig.h`: RP2350B GPIO mappings, PI gains, ADC constants, safety limits.
    - `ControllerApp.h` / `ControllerApp.cpp`: Master subsystem coordinator and loop scheduler.
    - `MachineStateMachine.h` / `MachineStateMachine.cpp`: Autonomous state transitions.
    - `ProfileEngine.h` / `ProfileEngine.cpp`: Reflow profiles and profile target curve.
    - `ThermalManager.h` / `ThermalManager.cpp`: Closed-loop PI controller, soft-start ramp, thermal runaway detector.
    - `TemperatureManager.h` / `TemperatureManager.cpp`: Multi-channel NTC acquisition and divergence detection.
    - `NtcSensor.h` / `NtcSensor.cpp`: Beta model temperature conversion and raw ADC bounds checks.
    - `HeaterController.h` / `HeaterController.cpp`: Time-proportioning SSR actuation strictly clamped to 40% duty cycle.
    - `FanController.h` / `FanController.cpp`: Dual fan 25 kHz PWM drive and cooling manager.
    - `Tachometer.h` / `Tachometer.cpp`: Fan tachometer pulse capture and RPM calculation.
    - `EncoderManager.h` / `EncoderManager.cpp`: Rotary encoder quadrature decoding and debounced push button.
    - `BuzzerManager.h` / `BuzzerManager.cpp`: Audible alerts and alarm patterns.
    - `ArgbManager.h` / `ArgbManager.cpp`: Status LED visual cues.
    - `UartTransport.h` / `UartTransport.cpp`: Non-blocking ring buffer UART transport.
    - `CommandParser.h` / `CommandParser.cpp`: Command validation, safety check, and dispatch.
    - `StatusPublisher.h` / `StatusPublisher.cpp`: Periodic STATUS packet broadcaster (5 Hz / 200 ms).
    - `WatchdogManager.h` / `WatchdogManager.cpp`: Hardware watchdog supervisor.
  - Display UI Terminal (`firmware/Tejasvini_UI/`):
    - `Tejasvini_UI.ino`: CrowPanel entry point.
    - `UiConfig.h`: Display, touch, and UART pin mappings.
    - `UiApp.h` / `UiApp.cpp`: UI lifecycle manager.
    - `DisplayManager.h` / `DisplayManager.cpp`: PicoDVI display driver and LVGL 8.3 bridge.
    - `TouchManager.h` / `TouchManager.cpp`: GT911 touch coordinate driver.
    - `StatusModel.h` / `StatusModel.cpp`: Cached state model for LVGL variables.
    - `ProtocolClient.h` / `ProtocolClient.cpp`: Non-blocking UART client and 5-second comms watchdog.
    - `UiBridge.h` / `UiBridge.cpp`: C-linkage bridge to UI events and variables.
    - `UiActions.cpp`: Handwritten implementations of EEZ Studio event callbacks (`action_on_start_stop`, `action_on_setpoint_inc`, etc.).
    - `UiVars.cpp`: Handwritten implementations of EEZ Studio variable getters/setters (`get_var_actual_temp`, etc.).
    - `ui/`: Strictly EEZ Studio generated UI files (`ui.c`, `screens.c`, `images.c`, `fonts.c`, `styles.c`, etc.).
  - EEZ Studio Export Update:
    - `ui.eez-project`: Updated `"destinationFolder": "firmware/Tejasvini_UI/ui"` to ensure future exports write directly to the UI firmware source tree.
  - Repository Cleanup:
    - Removed obsolete `src/` directory and root `Tejasvini.ino`.
    - Removed obsolete `docs/lvgl/`.
  - Automated Host Test Suite (`test/`):
    - Added CMake-based CTest suite with 4 test targets covering protocol parsing, key-value serialization, state machine transitions, and full UART end-to-end command/telemetry simulation.
- **Checkpoint Status:** CHECKPOINT #32 (Two-Target Architecture, Cleanup, EEZ Studio Export Alignment & Automated Tests Verified).
