// Copyright mzoesch. All rights reserved.

#include "Debug/DebugTraceSphere.h"
#include "Rhi/DebugTraceSphereShaderContext.h"
#include "Engine/World.h"
#include "User/LocalEgo.h"
#include "Framework/Eye.h"

namespace
{

Jafg::LDebugTraceSphereShaderContext* GetSphereShaderContext()
{
    /*
     * This object is indestructible and is therefore never freed. But that's okay.
     */
    static Jafg::LDebugTraceSphereShaderContext Context;
    return &Context;
}

} /* ~Anonymous Namespace */

void Jafg::LDebugTraceSphere::Draw(const LWorld& InContext, const LViewport& InViewport, const LEye& InEye) const
{
    if (::GetSphereShaderContext()->IsMeaningful() == false)
    {
        LOG_VERBOSE(LogRhi, "Creating DebugTraceSphereShaderContext.")
        ::GetSphereShaderContext()->Make();
    }

    LDebugTraceSphereShaderContextDrawArgs Args;
    Args.DegYFov = InEye.GetDegYFov();
    Args.ViewMatrix = InEye.GetViewMatrix();
    Args.Center = this->Center;
    Args.Radius = this->Radius;
    Args.Slices = this->VisualParams.Rings;
    Args.Stacks = this->VisualParams.Segments;
    Args.Color = this->VisualParams.Color;
    Args.Thickness = this->VisualParams.Thickness;
    ::GetSphereShaderContext()->Draw(InViewport, Args);

    return;
}
