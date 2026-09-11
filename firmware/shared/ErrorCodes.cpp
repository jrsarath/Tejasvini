#include "ErrorCodes.h"
#include <string.h>

const char* fault_code_to_string(FaultCode code) {
    switch (code) {
        case FAULT_NONE:             return "NONE";
        case FAULT_NTC1_OPEN:        return "NTC1_OPEN";
        case FAULT_NTC1_SHORT:       return "NTC1_SHORT";
        case FAULT_NTC2_OPEN:        return "NTC2_OPEN";
        case FAULT_NTC2_SHORT:       return "NTC2_SHORT";
        case FAULT_NTC3_OPEN:        return "NTC3_OPEN";
        case FAULT_NTC3_SHORT:       return "NTC3_SHORT";
        case FAULT_NTC_DIVERGENCE:   return "NTC_DIVERGENCE";
        case FAULT_OVERTEMP:         return "OVERTEMP";
        case FAULT_THERMAL_RUNAWAY:  return "THERMAL_RUNAWAY";
        case FAULT_COMM_TIMEOUT:     return "COMM_TIMEOUT";
        case FAULT_WATCHDOG_RESET:   return "WATCHDOG_RESET";
        case FAULT_HARDWARE_FAILURE: return "HARDWARE_FAILURE";
        case FAULT_SSR_FAIL:         return "SSR_FAIL";
        case FAULT_PROFILE_INVALID:  return "PROFILE_INVALID";
        default:                     return "UNKNOWN";
    }
}

FaultCode string_to_fault_code(const char* str) {
    if (!str) return FAULT_NONE;
    if (strcmp(str, "NONE") == 0)             return FAULT_NONE;
    if (strcmp(str, "NTC1_OPEN") == 0)        return FAULT_NTC1_OPEN;
    if (strcmp(str, "NTC1_SHORT") == 0)       return FAULT_NTC1_SHORT;
    if (strcmp(str, "NTC2_OPEN") == 0)        return FAULT_NTC2_OPEN;
    if (strcmp(str, "NTC2_SHORT") == 0)       return FAULT_NTC2_SHORT;
    if (strcmp(str, "NTC3_OPEN") == 0)        return FAULT_NTC3_OPEN;
    if (strcmp(str, "NTC3_SHORT") == 0)       return FAULT_NTC3_SHORT;
    if (strcmp(str, "NTC_DIVERGENCE") == 0)   return FAULT_NTC_DIVERGENCE;
    if (strcmp(str, "OVERTEMP") == 0)         return FAULT_OVERTEMP;
    if (strcmp(str, "THERMAL_RUNAWAY") == 0)  return FAULT_THERMAL_RUNAWAY;
    if (strcmp(str, "COMM_TIMEOUT") == 0)     return FAULT_COMM_TIMEOUT;
    if (strcmp(str, "WATCHDOG_RESET") == 0)   return FAULT_WATCHDOG_RESET;
    if (strcmp(str, "HARDWARE_FAILURE") == 0) return FAULT_HARDWARE_FAILURE;
    if (strcmp(str, "SSR_FAIL") == 0)         return FAULT_SSR_FAIL;
    if (strcmp(str, "PROFILE_INVALID") == 0)  return FAULT_PROFILE_INVALID;
    return FAULT_NONE;
}

const char* fault_severity_to_string(FaultSeverity severity) {
    switch (severity) {
        case SEVERITY_NONE:     return "NONE";
        case SEVERITY_INFO:     return "INFO";
        case SEVERITY_WARNING:  return "WARNING";
        case SEVERITY_CRITICAL: return "CRITICAL";
        case SEVERITY_FATAL:    return "FATAL";
        default:                return "NONE";
    }
}

FaultSeverity string_to_fault_severity(const char* str) {
    if (!str) return SEVERITY_NONE;
    if (strcmp(str, "NONE") == 0)     return SEVERITY_NONE;
    if (strcmp(str, "INFO") == 0)     return SEVERITY_INFO;
    if (strcmp(str, "WARNING") == 0)  return SEVERITY_WARNING;
    if (strcmp(str, "CRITICAL") == 0) return SEVERITY_CRITICAL;
    if (strcmp(str, "FATAL") == 0)    return SEVERITY_FATAL;
    return SEVERITY_NONE;
}
