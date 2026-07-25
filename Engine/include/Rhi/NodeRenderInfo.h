// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RenderInfo.h"
#include "Rhi/VisualInstance.h"

namespace Jafg
{

class JTextureSubsystem;
class JMaterialSubsystem;
class JFontSubsystem;

struct LNodeRenderInfo: LRenderInfo
{
    static constexpr u64 MaxBatchCount{16};

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
    const std::optional<LRect2F> Cull;

    TArray<std::pair<vk::Rect2D, u64>>& Batches;
    rhi::object_range<SSBO::VisualInstance>& VisualInstances;
    void BeginNewBatch(vk::Rect2D const& Scissor) const noexcept
    {
        this->Batches.emplace_back(Scissor, this->VisualInstances->size());
    }
    bool AddInstance(SSBO::VisualInstance Instance) const noexcept
    {
        if (this->Cull.has_value())
        {
            if (maths::aabb(*this->Cull, Instance.Rect) == false)
            {
                return false;
            }
        }
        this->VisualInstances->emplace_back(std::move(Instance));
        return true;
    }
};

} /* ~Namespace Jafg */
