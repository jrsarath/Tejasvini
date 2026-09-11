#include "ProfileEngine.h"

// 4 Profile Configurations (preserving existing vetted parameters):
// 1. MANUAL
// 2. LEAD FREE (SAC305): Preheat 150°C (90s) -> Soak 175°C (60s) -> Peak 245°C (30s dwell) -> Cool to 50°C
// 3. LEADED (Sn63Pb37): Preheat 130°C (80s) -> Soak 155°C (60s) -> Peak 215°C (30s dwell) -> Cool to 50°C
// 4. LOW TEMP (Sn42Bi58): Preheat 90°C (60s) -> Soak 115°C (45s) -> Peak 150°C (25s dwell) -> Cool to 45°C
static const ProfileStageConfig kProfileConfigs[4] = {
    {0, 0, 0, 0, 0, 0, 50, 0},             // MANUAL
    {150, 90, 175, 60, 245, 30, 50, 120},  // LEAD FREE
    {130, 80, 155, 60, 215, 30, 50, 120},  // LEADED
    {90, 60, 115, 45, 150, 25, 45, 90}     // LOW TEMP
};

static const char* kProfileNames[4] = {
    "MANUAL", "LEAD FREE", "LEADED", "LOW TEMP"
};

static const int kProfileTemps[4] = {
    0, 245, 215, 150
};

const ProfileStageConfig& ProfileEngine::getConfig(ReflowProfile profile) {
    int idx = (int)profile;
    if (idx >= 0 && idx < 4) {
        return kProfileConfigs[idx];
    }
    return kProfileConfigs[0];
}

const char* ProfileEngine::getName(ReflowProfile profile) {
    int idx = (int)profile;
    if (idx >= 0 && idx < 4) {
        return kProfileNames[idx];
    }
    return kProfileNames[0];
}

int ProfileEngine::getPresetTemp(ReflowProfile profile) {
    int idx = (int)profile;
    if (idx >= 0 && idx < 4) {
        return kProfileTemps[idx];
    }
    return 0;
}
