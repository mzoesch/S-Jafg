// Copyright mzoesch. All rights reserved.

#include "Lal.afx"
#include "Rhi/RendererInformation.h"
#include "Rhi/RhiVendorInclude.h"
#include "Widgets/InterfaceTypes.h"

i32 Jafg::RendererInformation::GetLimitTextureDimension()
{
    i32 Out = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &Out);
    return Out;
}

Jafg::LViewportSize Jafg::RendererInformation::GetLimitViewportDimensions()
{
    LViewportSize Out;
    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, Out.GetData());
    return Out;
}
