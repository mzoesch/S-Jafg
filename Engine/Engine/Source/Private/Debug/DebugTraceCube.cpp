// Copyright mzoesch. All rights reserved.

#include "Debug/DebugTraceCube.h"
#include "Engine/Framework/Eye.h"
#include "Engine/Framework/PersonaController.h"
#include "Rhi/DebugTraceCubeShaderContext.h"

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

void Jafg::LDebugTraceCube::Draw(const LWorld& InContext, const LViewport& InViewport, const LEye& InEye) const
{
    if (::GetCubeShaderContext()->IsMeaningful() == false)
    {
        LOG_VERBOSE(LogRhi, "Creating DebugTraceCubeShaderContext.")
        ::GetCubeShaderContext()->Make();
    }

    LDebugTraceCubeShaderContextDrawArgs Args;
    Args.DegYFov = InEye.GetDegYFov();
    Args.ViewMatrix.CopyFrom(InEye.GetViewMatrix());
    Args.BottomNearLeft = this->BottomNearLeft;
    Args.RelTopFarRight = this->TopRelFarRight;
    Args.Color = this->VisualParams.Color;
    Args.Thickness = this->VisualParams.Thickness;
    ::GetCubeShaderContext()->Draw(InViewport, Args);

    return;
}
