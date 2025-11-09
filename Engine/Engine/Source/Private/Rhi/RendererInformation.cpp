// Copyright mzoesch. All rights reserved.

#include "Lal.afx"
#include "Rhi/RendererInformation.h"
#include "Rhi/RhiVendorInclude.h"
#include "Widgets/InterfaceTypes.h"

i32 Jafg::RendererInformation::GetLimitTextureDimension()
{
    i32 Out = 500;
    // glGetIntegerv(GL_MAX_TEXTURE_SIZE, &Out);
    return Out;
}

LIntVector2 Jafg::RendererInformation::GetLimitViewportDimensions()
{
    LIntVector2 Out;
    // glGetIntegerv(GL_MAX_VIEWPORT_DIMS, Out.GetData());
    return Out;
}
