// Copyright mzoesch. All rights reserved.

#include "Platform/Surface.h"
#include "Widgets/Viewport.h"

void Jafg::LSurface::Initialize()
{
    this->SurfaceViewport = new LViewport();
    this->SurfaceViewport->Initialize();
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
