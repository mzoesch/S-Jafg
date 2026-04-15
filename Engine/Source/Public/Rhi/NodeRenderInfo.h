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
    inline static constexpr u64 MaxBatchCount{16};
    inline static constexpr u64 MaxInstanceCount{ 8'192 };

    LViewport const& Viewport;
    JTextureSubsystem const& TextureSubsystem;
    JMaterialSubsystem const& MaterialSubsystem;
    JFontSubsystem const& FontSubsystem;

    //#
    //# The translation that is recommended for children of a #WNode to use while drawing.
    //# This translation should be removed after said #WNode is finished drawing.
    //# This value is reset every frame.
    //#
    const LVec2F Translation;
    const TOptional<LRect2D> Cull;

    TArray<std::pair<vk::Rect2D, u64>>& Batches;
    TArray<LVisualInstance>& VisualInstances;
    inline void BeginNewBatch(vk::Rect2D const& Scissor) const noexcept
    {
        this->Batches.emplace_back(Scissor, this->VisualInstances.size());
    }
    inline bool AddInstance(LVisualInstance Instance) const noexcept
    {
        if (this->Cull.has_value())
        {
            if (maths::aabb(*this->Cull, Instance.Rect) == false)
            {
                return false;
            }
        }
        this->VisualInstances.emplace_back(std::move(Instance));
        return true;
    }
};

} /* ~Namespace Jafg */
