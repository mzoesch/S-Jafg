// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Debug/DebugTraceSphere.h"
#include "Rhi/DebugSphereShaderContext.h"
#include "Platform/Surface.h"
#include "Engine/World.h"
#include "User/LocalEgo.h"
#include "Engine/Engine.h"
#include "Engine/Framework/Eye.h"
#include "Engine/Framework/PersonaController.h"
#include "Engine/Framework/Pawn.h"

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

void Jafg::LDebugTraceSphere::Draw(const LWorld& InContext) const
{
    if (::GetSphereShaderContext()->IsMeaningful() == false)
    {
        LOG_VERBOSE(LogRhi, "Creating DebugTraceSphereShaderContext.")
        ::GetSphereShaderContext()->Make();
    }

    const LEye* Eye = GEngine->GetCheckedLocalEgo()->GetPossessed()->GetPossessed()->GetEye();

    LDebugTraceSphereShaderContextDrawArgs Args;
    Args.DegYFov = Eye->GetDegYFov();
    Args.ViewMatrix.CopyFrom(Eye->GetViewMatrix());
    Args.Center = this->Center;
    Args.Radius = this->Radius;
    Args.Slices = this->VisualParams.Rings;
    Args.Stacks = this->VisualParams.Segments;
    Args.Color = this->VisualParams.Color;
    ::GetSphereShaderContext()->Draw(*InContext.GetLocalEgo()->GetPrimarySurface()->GetViewport(), Args);

    return;
}
