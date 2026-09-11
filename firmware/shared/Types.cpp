#include "Types.h"
#include <string.h>

const char* machine_state_to_string(MachineState state) {
    switch (state) {
        case STATE_BOOTING:   return "BOOTING";
        case STATE_IDLE:      return "IDLE";
        case STATE_HEATING:   return "HEATING";
        case STATE_SOAKING:   return "SOAKING";
        case STATE_REFLOWING: return "REFLOWING";
        case STATE_COOLING:   return "COOLING";
        case STATE_PAUSED:    return "PAUSED";
        case STATE_COMPLETE:  return "COMPLETE";
        case STATE_FAULT:     return "FAULT";
        default:              return "UNKNOWN";
    }
}

MachineState string_to_machine_state(const char* str) {
    if (!str) return STATE_IDLE;
    if (strcmp(str, "BOOTING") == 0)   return STATE_BOOTING;
    if (strcmp(str, "IDLE") == 0)      return STATE_IDLE;
    if (strcmp(str, "HEATING") == 0)   return STATE_HEATING;
    if (strcmp(str, "SOAKING") == 0)   return STATE_SOAKING;
    if (strcmp(str, "REFLOWING") == 0) return STATE_REFLOWING;
    if (strcmp(str, "COOLING") == 0)   return STATE_COOLING;
    if (strcmp(str, "PAUSED") == 0)    return STATE_PAUSED;
    if (strcmp(str, "COMPLETE") == 0)  return STATE_COMPLETE;
    if (strcmp(str, "FAULT") == 0)     return STATE_FAULT;
    return STATE_IDLE;
}

const char* reflow_profile_to_string(ReflowProfile profile) {
    switch (profile) {
        case PROFILE_MANUAL:    return "MANUAL";
        case PROFILE_LEAD_FREE: return "LEAD_FREE";
        case PROFILE_LEADED:    return "LEADED";
        case PROFILE_LOW_TEMP:  return "LOW_TEMP";
        default:                return "MANUAL";
    }
}

ReflowProfile string_to_reflow_profile(const char* str) {
    if (!str) return PROFILE_MANUAL;
    if (strcmp(str, "MANUAL") == 0)    return PROFILE_MANUAL;
    if (strcmp(str, "LEAD_FREE") == 0) return PROFILE_LEAD_FREE;
    if (strcmp(str, "LEAD FREE") == 0) return PROFILE_LEAD_FREE;
    if (strcmp(str, "LEADED") == 0)    return PROFILE_LEADED;
    if (strcmp(str, "LOW_TEMP") == 0)  return PROFILE_LOW_TEMP;
    if (strcmp(str, "LOW TEMP") == 0)  return PROFILE_LOW_TEMP;
    return PROFILE_MANUAL;
}

const char* profile_stage_to_string(ProfileStage stage) {
    switch (stage) {
        case STAGE_NONE:    return "NONE";
        case STAGE_PREHEAT: return "PREHEAT";
        case STAGE_SOAK:    return "SOAK";
        case STAGE_REFLOW:  return "REFLOW";
        case STAGE_COOL:    return "COOL";
        case STAGE_DONE:    return "DONE";
        default:            return "NONE";
    }
}

ProfileStage string_to_profile_stage(const char* str) {
    if (!str) return STAGE_NONE;
    if (strcmp(str, "NONE") == 0)    return STAGE_NONE;
    if (strcmp(str, "PREHEAT") == 0) return STAGE_PREHEAT;
    if (strcmp(str, "SOAK") == 0)    return STAGE_SOAK;
    if (strcmp(str, "REFLOW") == 0)  return STAGE_REFLOW;
    if (strcmp(str, "COOL") == 0)    return STAGE_COOL;
    if (strcmp(str, "COOLING") == 0) return STAGE_COOL;
    if (strcmp(str, "DONE") == 0)    return STAGE_DONE;
    return STAGE_NONE;
}

const char* fan_mode_to_string(FanMode mode) {
    switch (mode) {
        case FAN_MODE_AUTO:   return "AUTO";
        case FAN_MODE_MANUAL: return "MANUAL";
        case FAN_MODE_OFF:    return "OFF";
        default:              return "AUTO";
    }
}

FanMode string_to_fan_mode(const char* str) {
    if (!str) return FAN_MODE_AUTO;
    if (strcmp(str, "AUTO") == 0)   return FAN_MODE_AUTO;
    if (strcmp(str, "MANUAL") == 0) return FAN_MODE_MANUAL;
    if (strcmp(str, "OFF") == 0)    return FAN_MODE_OFF;
    return FAN_MODE_AUTO;
}
