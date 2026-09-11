#ifndef TEJASVINI_SHARED_CONFIG_H_
#define TEJASVINI_SHARED_CONFIG_H_

#include <stdint.h>

/*******************************************************************************
 * Firmware Metadata
 ******************************************************************************/
#define FIRMWARE_NAME           "Tejasvini"
#define FIRMWARE_AUTHOR         "Sarath \"Delta\" Singh"
#define FIRMWARE_GITHUB         "https://github.com/jrsarath/Tejasvini"

/*******************************************************************************
 * UART Communication Specifications
 ******************************************************************************/
#define UART_BAUD_RATE          115200
#define PROTOCOL_MAX_FRAME_LEN  160
#define PROTOCOL_CMD_MAX_ARGS   8
#define PROTOCOL_MAX_ARG_LEN    32

// Communication heartbeat & timeout parameters
#define COMM_HEARTBEAT_INTERVAL_MS 1000  // UI sends ping or get_status every 1s
#define COMM_TIMEOUT_MS            5000  // Controller forces heater OFF if no packet for 5s while heating
#define COMM_STATUS_PERIOD_MS      500   // Controller broadcasts status at 2 Hz

/*******************************************************************************
 * Temperature Bounds & PI Controller Limits
 ******************************************************************************/
#define MIN_SETPOINT_TEMP       0        // °C
#define MAX_SETPOINT_TEMP       270      // °C
#define OVERTEMP_SHUTDOWN       280.0f   // Emergency shutoff limit (°C)

#define NTC_MIN_VALID_TEMP      -20.0f   // Minimum physically credible sensor reading (°C)
#define NTC_MAX_VALID_TEMP      300.0f   // Maximum physically credible sensor reading (°C)

#define ADC_MIN_VALID_COUNTS    50       // 12-bit ADC minimum (detects open circuit / GND pull)
#define ADC_MAX_VALID_COUNTS    4050     // 12-bit ADC maximum (detects short to 3.3V)

#define MAX_STARTUP_NTC_DIFF    15.0f    // Max divergence between sensors allowed to start heating (°C)
#define MAX_NTC_DIFF            35.0f    // Max divergence allowed while heating (°C)

// Maximum SSR duty cycle strictly clamped to 40%
#define MAX_DUTY_CYCLE          40.0f    // %
#define MIN_DUTY_CYCLE          0.0f     // %

// Thermal runaway check interval & threshold
#define THERMAL_RUNAWAY_PERIOD_MS 18000  // 18 seconds
#define THERMAL_RUNAWAY_MIN_RISE  2.0f   // Minimum required rise in °C

/*******************************************************************************
 * Fan Control Parameters
 ******************************************************************************/
#define FAN_PWM_FREQUENCY_HZ    25000    // Standard 25 kHz 4-wire fan PWM
#define FAN_DEFAULT_IDLE_PWM    0        // Fan default idle speed (0%)
#define FAN_COOLING_PWM         100      // Fan speed during active cooling stage (100%)

#endif // TEJASVINI_SHARED_CONFIG_H_
