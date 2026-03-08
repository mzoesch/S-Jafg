// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RenderInfo.h"
#include "Rhi/VisualInstance.h"

namespace Jafg
{

class JTextureSubsystem;
class JMaterialSubsystem;
class JFontSubsystem;

struct LNodeRenderInfo : public LRenderInfo
{
    LViewport const& Viewport;
    mutable TArray<LVisualInstance> VisualInstances;

    JTextureSubsystem const& TextureSubsystem;
    JMaterialSubsystem const& MaterialSubsystem;
    JFontSubsystem const& FontSubsystem;
};

} /* ~Namespace Jafg */
