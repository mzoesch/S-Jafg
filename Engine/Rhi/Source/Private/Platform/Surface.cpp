// Copyright mzoesch. All rights reserved.

#include "Platform/Surface.h"
#include "Widgets/Viewport.h"

void Jafg::LSurface::Initialize()
{
    this->Viewport = new LViewport();
}

void Jafg::LSurface::OnClear()
{
}

void Jafg::LSurface::OnUpdate()
{
}

void Jafg::LSurface::TearDown()
{
    if (ensure(this->Viewport))
    {
        this->Viewport->TearDown();
        delete this->Viewport;
        this->Viewport = nullptr;
    }

    return;
}
