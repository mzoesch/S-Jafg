// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

#if !WITH_LOCAL_LAYER
    #error "WITH_LOCAL_LAYER is requried."
#endif /* !WITH_LOCAL_LAYER */

namespace Jafg
{

struct LEye_v2
{
    f32 DegYFov{ 90.0f };
    f32 NearFrustum{ 0.1f };
    f32 FarFrustum{ 10.0f };
    LVector3F Location;
    LVector3F Front;
    LVector3F Up;
};

} /* ~Namespace Jafg */
