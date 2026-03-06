// Copyright mzoesch. All rights reserved.

#include "Widgets/Region.h"
#include "Engine/Engine.h"
#include "Rhi/VisualInstance.h"
#include "Rhi/GraphicsPipelineFactory.h"
#include "Rhi/NodeRenderInfo.h"

void Jafg::WRegion::Draw(LNodeRenderInfo const& Info) const
{
    Info.VisualInstances.emplace_back(LVisualInstance{
        .Rect = {this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Viewport), this->GetAnchoredSize_v2()},
        .Tint = this->Brush.Tint.ToVector4(),
        .Radii = this->Brush.Radii,
        .OutlineTint = this->Brush.OutlineTint.ToVector4(),
        .TextureTint = this->Brush.ImageTint.ToVector4(),
        .TexCoordRect = {0.0f, 0.0f, 1.0f, 1.0f},
        .OutlineThickness = this->Brush.OutlineThickness,
        });

    Super::Draw(Info);

    return;
}
