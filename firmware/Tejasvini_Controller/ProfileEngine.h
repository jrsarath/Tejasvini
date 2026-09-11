#ifndef TEJASVINI_PROFILE_ENGINE_H_
#define TEJASVINI_PROFILE_ENGINE_H_

#include "../shared/Types.h"
#include <stdint.h>

class ProfileEngine {
public:
    static const ProfileStageConfig& getConfig(ReflowProfile profile);
    static const char* getName(ReflowProfile profile);
    static int getPresetTemp(ReflowProfile profile);
};

#endif // TEJASVINI_PROFILE_ENGINE_H_
