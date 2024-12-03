// Copyright mzoesch. All rights reserved.

#pragma once

#if !PLATFORM_DESKTOP
    #error "Tried to include RendererApplier.h on a platform that is not desktop."
#endif /* !PLATFORM_DESKTOP */

#include "CoreAfx.h"
#include "Platform/Surface.h"

namespace Jafg::RendererApplier
{

bool InitializeGlfw();
bool IsGlfwInitialized();
bool ApplyOpenGlToWindow(LNativeWindow* Window);

} /* ~Namespace Jafg::RendererApplier */
