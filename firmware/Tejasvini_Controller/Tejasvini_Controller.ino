/*******************************************************************************
 * Tejasvini Controller Firmware
 * Open-Source Heatplate Controller for SMT Reflow Soldering
 *
 * Board:     RP2350B Machine Controller Board
 * Subsystem: Machine Authority, Thermal Control, Safety, Profiles & Sensors
 * Protocol:  UART @ 115200 baud to CrowPanel UI
 ******************************************************************************/

#include "ControllerApp.h"

static ControllerApp g_controller_app;

void setup() { g_controller_app.setup(); }

void loop() { g_controller_app.loop(); }
