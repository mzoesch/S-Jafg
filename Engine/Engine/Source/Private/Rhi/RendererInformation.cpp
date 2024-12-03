// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Rhi/RendererInformation.h"
#include "Rhi/RhiVendorInclude.h"
#include "Widgets/InterfaceTypes.h"

int32 Jafg::RendererInformation::GetLimitTextureDimension()
{
    int32 Out = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &Out);
    return Out;
}

Jafg::LViewportSize Jafg::RendererInformation::GetLimitViewportDimensions()
{
    LViewportSize Out;
    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, Out.GetData());
    return Out;
}
