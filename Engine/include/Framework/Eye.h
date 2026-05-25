// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

#if !WITH_LOCAL_LAYER
    #error "WITH_LOCAL_LAYER is requried."
#endif /* !WITH_LOCAL_LAYER */

namespace Jafg
{

struct LEye_v2
{
    LWorldReal VertFov{ 1 };
    LWorldReal NearFrustum{ 0.1 };
    LWorldReal FarFrustum{ 10.0 };
    LWorldVec3 Translation;
    LWorldVec3 Front;
    LWorldVec3 Up;
};

} /* ~Namespace Jafg */
