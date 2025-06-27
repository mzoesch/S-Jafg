// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

class LViewport;
class LSurfaceBase;
class LSurfaceProxy;

#if JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER
    class LSurfaceGlfw3;
    typedef LSurfaceGlfw3 LSurface;
#elif PLATFORM_USES_JAVA_SCRIPT_FRONTEND
    class   LSurfaceDom;
    typedef LSurfaceDom LSurface;
#else /* PLATFORM_USES_JAVA_SCRIPT_FRONTEND */
    #error "Could not resolve PLATFORM."
#endif /* !PLATFORM_USES_JAVA_SCRIPT_FRONTEND */

} /* ~Namespace Jafg */
