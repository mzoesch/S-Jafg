// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

#if !JAFG_WITH_LOCAL_LAYER
    #error "JAFG_WITH_LOCAL_LAYER is requried."
#endif /* !JAFG_WITH_LOCAL_LAYER */

namespace Jafg
{

struct LEye_v2
{
    LWorldReal VertFov{ static_cast<f32>(1.0) };
    LWorldReal NearFrustum{ static_cast<LWorldReal>(0.1) };
    LWorldReal FarFrustum{ static_cast<f32>(10.0) };
    LWorldVec3 Translation;
    LWorldVec3 Front;
    LWorldVec3 Up;
};

} /* ~Namespace Jafg */
