// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

class LViewport;
class LSurface;

#if PLATFORM_USES_GLFW3_ABSTRACTION_LAYER
    class LSurfaceGlfw3;
    typedef LSurfaceGlfw3 LCurrentSurface;
#elif PLATFORM_USES_JAVA_SCRIPT_FRONTEND
    class   LSurfaceDom;
    typedef LSurfaceDom LCurrentSurface;
#else /* PLATFORM_USES_JAVA_SCRIPT_FRONTEND */
    #error "Could not resolve PLATFORM."
#endif /* !PLATFORM_USES_JAVA_SCRIPT_FRONTEND */

} /* ~Namespace Jafg */
