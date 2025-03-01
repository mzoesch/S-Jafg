// Copyright mzoesch. All rights reserved.

#include "Debug/DebugTraceCube.h"
#include "Engine/Engine.h"
#include "Engine/Framework/Eye.h"
#include "Engine/Framework/Pawn.h"
#include "Engine/Framework/PersonaController.h"
#include "Platform/Surface.h"
#include "Rhi/DebugTraceCubeShaderContext.h"
#include "User/LocalEgo.h"

namespace
{

Jafg::LDebugTraceCubeShaderContext* GetCubeShaderContext()
{
    /*
     * This object is indestructible and is therefore never freed. But that's okay.
     */
    static Jafg::LDebugTraceCubeShaderContext Context;
    return &Context;
}

} /* ~Anonymous Namespace */

void Jafg::LDebugTraceCube::Draw(const LWorld& InContext) const
{
    if (::GetCubeShaderContext()->IsMeaningful() == false)
    {
        LOG_VERBOSE(LogRhi, "Creating DebugTraceCubeShaderContext.")
        ::GetCubeShaderContext()->Make();
    }

    const LEye* Eye = GEngine->GetLocalEgoChecked()->GetPossessed()->GetPossessed()->GetEye();

    LDebugTraceCubeShaderContextDrawArgs Args;
    Args.DegYFov = Eye->GetDegYFov();
    Args.ViewMatrix.CopyFrom(Eye->GetViewMatrix());
    Args.BottomNearLeft = this->BottomNearLeft;
    Args.RelTopFarRight = this->TopRelFarRight;
    Args.Color = this->VisualParams.Color;
    Args.Thickness = this->VisualParams.Thickness;
    ::GetCubeShaderContext()->Draw(InContext.GetLocalEgo()->GetHud()->GetSurfaces()[0].GetViewport(), Args);

    return;
}
