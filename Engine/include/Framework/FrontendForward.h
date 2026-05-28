// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

class LFrontendBase;

#if JAFG_PLATFORM_DESKTOP
    class LFrontendNativeDesktop;
#endif /* JAFG_PLATFORM_DESKTOP */

class LFrontendVk;

typedef LFrontendVk LFrontend;

} /* ~Namespace Jafg */
