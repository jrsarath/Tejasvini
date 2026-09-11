/*******************************************************************************
 * Tejasvini UI Firmware
 * Open-Source Heatplate Controller for SMT Reflow Soldering
 *
 * Board:     Elecrow CrowPanel 4.3" Pico DVI Display (RP2040)
 * Subsystem: HMI Terminal (PicoDVI, GT911 Touch, LVGL 8.3 UI)
 * Protocol:  UART @ 115200 baud to RP2350B Controller
 ******************************************************************************/

#include "UiApp.h"

static UiApp g_ui_app;

void setup() { g_ui_app.setup(); }

void loop() { g_ui_app.loop(); }
