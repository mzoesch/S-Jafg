// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Debug/DebugTraceLine.h"
#include "Rhi/DebugTraceLineShaderContext.h"
#include "Engine/World.h"
#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "Engine/Engine.h"
#include "Engine/Framework/Eye.h"
#include "Engine/Framework/PersonaController.h"
#include "Engine/Framework/Pawn.h"

namespace
{

Jafg::LDebugTraceLineShaderContext* GetLineShaderContext()
{
    /*
     * This object is indestructible and is therefore never freed. But that's okay.
     */
    static Jafg::LDebugTraceLineShaderContext Context;
    return &Context;
}

} /* ~Anonymous Namespace */

void Jafg::LDebugTraceLine::Draw(const LWorld& InContext) const
{
    if (::GetLineShaderContext()->IsMeaningful() == false)
    {
        LOG_VERBOSE(LogRhi, "Creating DebugTraceLineShaderContext.")
        ::GetLineShaderContext()->Make();
    }

    const LEye* Eye = GEngine->GetCheckedLocalEgo()->GetPossessed()->GetPossessed()->GetEye();

    LDebugTraceLineShaderContextDrawArgs Args;
    Args.DegYFov = Eye->GetDegYFov();
    Args.ViewMatrix.CopyFrom(Eye->GetViewMatrix());
    Args.Start = this->Start;
    Args.End   = this->End;
    Args.Color = this->VisualParams.Color;
    ::GetLineShaderContext()->Draw(InContext.GetLocalEgo()->GetPrimarySurface()->GetViewport(), Args);

    return;
}

void Jafg::LDebugTraceLine::DrawLine(const LWorld& InContext, const LVector& InStart, const LVector& InEnd, const LColor& InColor, const uint8 Thickness, const bool bUseCache)
{
    if (::GetLineShaderContext()->IsMeaningful() == false)
    {
        LOG_VERBOSE(LogRhi, "Creating DebugTraceLineShaderContext.")
        ::GetLineShaderContext()->Make();
    }

    static LDebugTraceLineShaderContextDrawArgs Args;
    if (bUseCache == false)
    {
        const LEye* Eye = GEngine->GetCheckedLocalEgo()->GetPossessed()->GetPossessed()->GetEye();
        Args.DegYFov = Eye->GetDegYFov();
        Args.ViewMatrix.CopyFrom(Eye->GetViewMatrix());
    }
    Args.Start = InStart;
    Args.End   = InEnd;
    Args.Color = InColor;
    Args.Thickness = Thickness;

    ::GetLineShaderContext()->Draw(InContext.GetLocalEgo()->GetPrimarySurface()->GetViewport(), Args);

    return;
}
