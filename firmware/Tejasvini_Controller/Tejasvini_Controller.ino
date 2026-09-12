/*******************************************************************************
 * Tejasvini Controller Firmware
 * Open-Source Heatplate Controller for SMT Reflow Soldering
 *
 * Board:     RP2350B Machine Controller Board
 * Subsystem: Machine Authority, Thermal Control, Safety, Profiles & Sensors
 * Protocol:  UART @ 115200 baud to CrowPanel UI
 ******************************************************************************/

#include "ControllerApp.h"

static ControllerApp controller_app;

void setup() { controller_app.setup(); }

void loop() { controller_app.loop(); }
