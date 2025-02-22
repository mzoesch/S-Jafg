// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

class LViewport;
class LSurface;

#if PLATFORM_USES_GLFW3_ABSTRACTION_LAYER
    class LGlfw3Surface;
    typedef LGlfw3Surface LCurrentSurface;
#elif PLATFORM_USES_JAVA_SCRIPT_FRONTEND
    class   LWasmSurface;
    typedef LWasmSurface LCurrentSurface;
#else /* PLATFORM_USES_JAVA_SCRIPT_FRONTEND */
    #error "Could not resolve PLATFORM."
#endif /* !PLATFORM_USES_JAVA_SCRIPT_FRONTEND */

} /* ~Namespace Jafg */
