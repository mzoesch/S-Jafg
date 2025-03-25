// Copyright mzoesch. All rights reserved.

#include "Debug/DebugTraceLine.h"
#include "Rhi/DebugTraceLineShaderContext.h"
#include "Engine/World.h"
#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "Framework/Eye.h"

namespace
{

const Jafg::LViewport* CacheViewport = nullptr;
Jafg::LDebugTraceLineShaderContextDrawArgs CacheArgs;
Jafg::LDebugTraceLineShaderContext* GetLineShaderContext()
{
    /*
     * This object is indestructible and is therefore never freed. But that's okay.
     */
    static Jafg::LDebugTraceLineShaderContext Context;
    return &Context;
}

} /* ~Anonymous Namespace */

void Jafg::LDebugTraceLine::Draw(const LWorld& InContext, const LViewport& InViewport, const LEye& InEye) const
{
    if (::GetLineShaderContext()->IsMeaningful() == false)
    {
        LOG_VERBOSE(LogRhi, "Creating DebugTraceLineShaderContext.")
        ::GetLineShaderContext()->Make();
    }

    LDebugTraceLineShaderContextDrawArgs Args;
    Args.DegYFov = InEye.GetDegYFov();
    Args.ViewMatrix.CopyFrom(InEye.GetViewMatrix());
    Args.Start = this->Start;
    Args.End   = this->End;
    Args.Color = this->VisualParams.Color;
    ::GetLineShaderContext()->Draw(InViewport, Args);

    return;
}

void Jafg::LDebugTraceLine::DrawLine(
    const LViewport& InViewport, const LEye& InEye,
    const LVector& InStart, const LVector& InEnd,
    const LColor& InColor, const u8 Thickness
)
{
    if (::GetLineShaderContext()->IsMeaningful() == false)
    {
        LOG_VERBOSE(LogRhi, "Creating DebugTraceLineShaderContext.")
        ::GetLineShaderContext()->Make();
    }

    CacheViewport = &InViewport;

    CacheArgs.DegYFov = InEye.GetDegYFov();
    CacheArgs.ViewMatrix.CopyFrom(InEye.GetViewMatrix());
    CacheArgs.Start = InStart;
    CacheArgs.End   = InEnd;
    CacheArgs.Color = InColor;
    CacheArgs.Thickness = Thickness;

    ::GetLineShaderContext()->Draw(InViewport, CacheArgs);

    return;
}

void Jafg::LDebugTraceLine::DrawLineCachedValues(const LVector& InStart, const LVector& InEnd, const LColor& InColor, const u8 Thickness)
{
    check( ::GetLineShaderContext()->IsMeaningful() )

    CacheArgs.Start     = InStart;
    CacheArgs.End       = InEnd;
    CacheArgs.Color     = InColor;
    CacheArgs.Thickness = Thickness;

    ::GetLineShaderContext()->Draw(*CacheViewport, CacheArgs);

    return;
}
