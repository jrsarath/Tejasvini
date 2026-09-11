#ifndef TEJASVINI_SHARED_TYPES_H_
#define TEJASVINI_SHARED_TYPES_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Authoritative machine operational states.
 */
typedef enum {
    STATE_BOOTING = 0,
    STATE_IDLE = 1,
    STATE_HEATING = 2,
    STATE_SOAKING = 3,
    STATE_REFLOWING = 4,
    STATE_COOLING = 5,
    STATE_PAUSED = 6,
    STATE_COMPLETE = 7,
    STATE_FAULT = 8
} MachineState;

/**
 * @brief Available reflow profile types.
 */
typedef enum {
    PROFILE_MANUAL = 0,
    PROFILE_LEAD_FREE = 1,
    PROFILE_LEADED = 2,
    PROFILE_LOW_TEMP = 3
} ReflowProfile;

/**
 * @brief Sub-stages during reflow profile execution.
 */
typedef enum {
    STAGE_NONE = 0,
    STAGE_PREHEAT = 1,
    STAGE_SOAK = 2,
    STAGE_REFLOW = 3,
    STAGE_COOL = 4,
    STAGE_DONE = 5
} ProfileStage;

/**
 * @brief Operating modes for cooling fans.
 */
typedef enum {
    FAN_MODE_AUTO = 0,
    FAN_MODE_MANUAL = 1,
    FAN_MODE_OFF = 2
} FanMode;

/**
 * @brief Profile configuration parameters for automated reflow curves.
 */
typedef struct {
    int preheat_temp;        // Target preheat temperature (°C)
    uint32_t preheat_time_s; // Nominal preheat duration (seconds)
    int soak_temp;           // Target soak temperature (°C)
    uint32_t soak_time_s;    // Soak duration (seconds)
    int peak_temp;           // Target peak reflow temperature (°C)
    uint32_t peak_dwell_s;   // Peak dwell duration (seconds)
    int cool_temp;           // Target safe cooling temperature (°C)
    uint32_t cool_time_s;    // Nominal cooling duration (seconds)
} ProfileStageConfig;

/**
 * @brief Helper functions to convert enums to human-readable strings.
 */
const char* machine_state_to_string(MachineState state);
MachineState string_to_machine_state(const char* str);

const char* reflow_profile_to_string(ReflowProfile profile);
ReflowProfile string_to_reflow_profile(const char* str);

const char* profile_stage_to_string(ProfileStage stage);
ProfileStage string_to_profile_stage(const char* str);

const char* fan_mode_to_string(FanMode mode);
FanMode string_to_fan_mode(const char* str);

#ifdef __cplusplus
}
#endif

#endif // TEJASVINI_SHARED_TYPES_H_
