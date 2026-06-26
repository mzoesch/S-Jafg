// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

#ifndef JAFG_MAX_FRAMES_IN_FLIGHT
    //# This engine is for chill stuff. So triple buffering is okay. :)
    #define JAFG_MAX_FRAMES_IN_FLIGHT                                3
#endif /* !JAFG_MAX_FRAMES_IN_FLIGHT */

namespace rhi
{

//#
//# The desired maximum number of frames in flight.
//# @note This is the desired value and not the actual value. Hardware vendors may limit this value to a lower number.
//#       It is guaranteed that the actual number if always below or equal to this value.
//#       The actual frames in flight can be found under #LFrontendVk::Vk_GetNumberOfFramesInFlight.
//#
static constexpr u32 max_frames_in_flight{ JAFG_MAX_FRAMES_IN_FLIGHT };

//# Stack array for per-frame data.
template<typename T> using frame_array = std::array<T, max_frames_in_flight>;

#ifndef JAFG_VK_FRAME_ARRAY_INIT
    #if JAFG_MAX_FRAMES_IN_FLIGHT == 1
        #define JAFG_VK_FRAME_ARRAY_INIT(DefaultValue) JAFG_INIT_ONE(JAFG_COMBINE_ANY_OUTER(DefaultValue))
    #elif JAFG_MAX_FRAMES_IN_FLIGHT == 2
        #define JAFG_VK_FRAME_ARRAY_INIT(DefaultValue) JAFG_INIT_TWO(JAFG_COMBINE_ANY_OUTER(DefaultValue))
    #elif JAFG_MAX_FRAMES_IN_FLIGHT == 3
        #define JAFG_VK_FRAME_ARRAY_INIT(DefaultValue) JAFG_INIT_THREE(JAFG_COMBINE_ANY_OUTER(DefaultValue))
    #else /* JAFG_MAX_FRAMES_IN_FLIGHT */
        #error "Unsupported value for JAFG_MAX_FRAMES_IN_FLIGHT. Only 1, 2, and 3 are supported."
    #endif /* !JAFG_MAX_FRAMES_IN_FLIGHT */
#endif /* !JAFG_VK_FRAME_ARRAY_INIT */

} /* ~Namespace rhi */
