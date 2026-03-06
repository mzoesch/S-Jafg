// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RenderInfo.h"
#include "Rhi/VisualInstance.h"

namespace Jafg
{

struct LNodeRenderInfo : public LRenderInfo
{
    LViewport const& Viewport;
    mutable TArray<LVisualInstance> VisualInstances;
};

} /* ~Namespace Jafg */
