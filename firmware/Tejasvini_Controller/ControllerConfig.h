#ifndef TEJASVINI_CONTROLLER_CONFIG_H_
#define TEJASVINI_CONTROLLER_CONFIG_H_

#if __has_include("../shared/Config.h")
#include "../shared/Config.h"
#else
#include "shared/Config.h"
#endif
#include <Arduino.h>

/*******************************************************************************
 * Hardware Pinout Configuration (RP2350B Machine Controller)
 ******************************************************************************/
#define PIN_UART_TX         32   // Hardware UART TX (GPIO 32 to CrowPanel GP1)
#define PIN_UART_RX         33   // Hardware UART RX (GPIO 33 to CrowPanel GP0)

#define PIN_SSR             22   // Solid State Relay 3.3V Logic Drive (GPIO 22)
#define PIN_RELAY           23   // Auxiliary Safety Relay Output (GPIO 23)

#define PIN_NTC1            26   // Primary NTC Thermistor ADC Input (ADC0, GPIO 26)
#define PIN_NTC2            27   // Secondary NTC Thermistor ADC Input (ADC1, GPIO 27)
#define PIN_NTC3            28   // Auxiliary NTC Thermistor ADC Input (ADC2, GPIO 28)

#define PIN_ENA             2    // Rotary Encoder Phase A (GPIO 2)
#define PIN_ENB             3    // Rotary Encoder Phase B (GPIO 3)
#define PIN_EBT             4    // Rotary Encoder Push-Button (GPIO 4, Active LOW)

#define PIN_FAN1_PWM        6    // Fan 1 PWM Control (GPIO 6)
#define PIN_FAN1_TACH       7    // Fan 1 Tachometer Sense (GPIO 7)
#define PIN_FAN2_PWM        8    // Fan 2 PWM Control (GPIO 8)
#define PIN_FAN2_TACH       9    // Fan 2 Tachometer Sense (GPIO 9)

#define PIN_BUZZER          10   // Audible Piezo Buzzer Output (GPIO 10)
#define PIN_ARGB            11   // Status WS2812B / NeoPixel ARGB LED (GPIO 11)

/*******************************************************************************
 * NTC Circuit Constants
 ******************************************************************************/
#define R_DIVIDER           100000.0f // 100k Ohms precision series resistor
#define R0_THERMISTOR       100000.0f // 100k Ohms at 25°C
#define BETA_COEFF          3950.0f   // NTC Beta coefficient
#define T0_KELVIN           298.15f   // 25°C in Kelvin

/*******************************************************************************
 * Thermal PI Loop & Timing Parameters
 ******************************************************************************/
#define CONTROL_PERIOD_MS   100       // 10 Hz control loop
#define SSR_WINDOW_MS       1000      // 1-second time-proportioning window
#define RAMP_RATE_DEG_PER_S 1.5f      // Soft-start ramp rate (°C/sec)

#define KP_GAIN             1.5f      // Proportional gain
#define KI_GAIN             0.02f     // Integral gain

#define BTN_SHORT_PRESS_MS  80        // Short press debouncing duration (ms)
#define BTN_LONG_PRESS_MS   1000      // Long press duration (ms)

#define WATCHDOG_TIMEOUT_MS 8000      // Microcontroller hardware watchdog period

#endif // TEJASVINI_CONTROLLER_CONFIG_H_
