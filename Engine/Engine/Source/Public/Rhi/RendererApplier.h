// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Platform/Surface.h"

namespace Jafg::RendererApplier
{

bool InitializeGlfw();
bool IsGlfwInitialized();
bool ApplyOpenGlToWindow(LNativeWindow* Window);

} /* ~Namespace Jafg::RendererApplier */
