// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

#ifndef JAFG_VK_DESIRED_MAX_FRAMES_IN_FLIGHT
    //# This engine is for chill stuff. So triple buffering is okay. :)
    #define JAFG_VK_DESIRED_MAX_FRAMES_IN_FLIGHT                                3
#endif /* !JAFG_VK_DESIRED_MAX_FRAMES_IN_FLIGHT */

namespace Jafg
{

//#
//# The desired maximum number of frames in flight.
//# @note This is the desired value and not the actual value. Hardware vendors may limit this value to a lower number
//#       or a value higher than this.
//#       The actual frames in flight can be found under #LFrontendVk::Vk_GetNumberOfFramesInFlight.
//#
static constexpr u32 Vk_DesiredMaxFramesInFlight{ JAFG_VK_DESIRED_MAX_FRAMES_IN_FLIGHT };

} /* ~Namespace Jafg */
