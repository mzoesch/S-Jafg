// Copyright mzoesch. All rights reserved.

#include "Platform/Surface.h"
#include "Platform/PlatformMisc.h"
#include "Widgets/Viewport.h"

DECLARE_INLINE_LOG_CATEGORY(LogSurface, Trace)

void Jafg::LSurface::Initialize()
{
    PlatformMisc::InvalidateCachedValues();

    this->SurfaceViewport = new LViewport();
    this->SurfaceViewport->Initialize();

    LOG_VERBOSE(LogSurface, "Created surface viewport.")
    LOG_INFO(LogSurface, "Platform stats:")
    LOG_INFO(LogSurface, " - Physical monitors({}).", GPlatformMisc->NumberOfPhysicalViewports)
    for (const LPhysicalViewport& Viewport : GPlatformMisc->PhysicalViewports)
    {
        LOG_INFO(LogSurface, "  - {}", Viewport.ToString())
    }

    return;
}

void Jafg::LSurface::OnClear()
{
}

void Jafg::LSurface::OnUpdate()
{
    this->SurfaceViewport->Draw();
}

void Jafg::LSurface::TearDown()
{
    if (ensure(this->SurfaceViewport))
    {
        this->SurfaceViewport->TearDown();
        delete this->SurfaceViewport;
        this->SurfaceViewport = nullptr;
    }

    return;
}
