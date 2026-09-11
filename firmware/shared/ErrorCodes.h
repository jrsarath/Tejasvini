#ifndef TEJASVINI_SHARED_ERROR_CODES_H_
#define TEJASVINI_SHARED_ERROR_CODES_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Machine fault codes.
 */
typedef enum {
    FAULT_NONE = 0,
    FAULT_NTC1_OPEN = 1,
    FAULT_NTC1_SHORT = 2,
    FAULT_NTC2_OPEN = 3,
    FAULT_NTC2_SHORT = 4,
    FAULT_NTC3_OPEN = 5,
    FAULT_NTC3_SHORT = 6,
    FAULT_NTC_DIVERGENCE = 7,
    FAULT_OVERTEMP = 8,
    FAULT_THERMAL_RUNAWAY = 9,
    FAULT_COMM_TIMEOUT = 10,
    FAULT_WATCHDOG_RESET = 11,
    FAULT_HARDWARE_FAILURE = 12,
    FAULT_SSR_FAIL = 13,
    FAULT_PROFILE_INVALID = 14
} FaultCode;

/**
 * @brief Severity level of a fault.
 */
typedef enum {
    SEVERITY_NONE = 0,
    SEVERITY_INFO = 1,
    SEVERITY_WARNING = 2,
    SEVERITY_CRITICAL = 3,
    SEVERITY_FATAL = 4
} FaultSeverity;

const char* fault_code_to_string(FaultCode code);
FaultCode string_to_fault_code(const char* str);

const char* fault_severity_to_string(FaultSeverity severity);
FaultSeverity string_to_fault_severity(const char* str);

#ifdef __cplusplus
}
#endif

#endif // TEJASVINI_SHARED_ERROR_CODES_H_
