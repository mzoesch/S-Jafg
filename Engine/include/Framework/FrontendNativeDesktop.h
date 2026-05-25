// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Frontend.h"

#if !JAFG_PLATFORM_DESKTOP
    #error Platform is not desktop.
#endif /* !JAFG_PLATFORM_DESKTOP */

namespace Jafg
{

class LFrontendNativeDesktop : public LFrontendBase
{
public:

    ENGINE_API void OpenDirectory(LPath const& Directory) const;
    ENGINE_API void OpenTerminal(LPath const& Directory) const;
};

} /* ~Namespace Jafg */
