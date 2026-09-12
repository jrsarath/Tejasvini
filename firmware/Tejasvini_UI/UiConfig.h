#ifndef TEJASVINI_UI_CONFIG_H_
#define TEJASVINI_UI_CONFIG_H_

#include "Config.h"
#include <Arduino.h>

/*******************************************************************************
 * Hardware Pinout Configuration (CrowPanel RP2040 DVI Terminal)
 ******************************************************************************/
#define PIN_UART_TX          0   // Hardware UART0 TX (GPIO 0 to Controller GP33)
#define PIN_UART_RX          1   // Hardware UART0 RX (GPIO 1 to Controller GP32)

#define PIN_BACKLIGHT        24  // Display Backlight Control (Active LOW, GPIO 24)

// GT911 Capacitive Touch Controller (I2C)
#define TOUCH_GT911_SDA      20  // GPIO 20 (4.7k pullup)
#define TOUCH_GT911_SCL      21  // GPIO 21 (4.7k pullup)
#define TOUCH_GT911_INT      25  // GPIO 25
#define TOUCH_GT911_RST      29  // GPIO 29

/*******************************************************************************
 * Display Specifications (Elecrow CrowPanel RTD2281 4.3" DVI Display)
 * Hardware DVI Resolution: 400x240 @ 60Hz (scaled 2x to 800x480 by RTD2281)
 * Logical UI Resolution:  240x400 (Portrait Mode, 270° hardware rotation)
 ******************************************************************************/
#define SCREEN_WIDTH         240 // Logical UI width (pixels, portrait)
#define SCREEN_HEIGHT        400 // Logical UI height (pixels, portrait)
#define DISPLAY_WIDTH        400 // Hardware DVI width (pixels, landscape)
#define DISPLAY_HEIGHT       240 // Hardware DVI height (pixels, landscape)
#define DISPLAY_ROTATION     3   // 270° Rotation for Portrait

#define LVGL_BUF_LINES       8   // Partial buffer height (3,840 bytes)

#endif // TEJASVINI_UI_CONFIG_H_
