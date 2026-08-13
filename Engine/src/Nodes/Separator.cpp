// Copyright mzoesch. All rights reserved.

#include "Nodes/Separator.h"
#include "Rhi/NodeRenderInfo.h"

void Jafg::WHSeparator::Draw(LNodeRenderInfo const& Info) const
{
    Super::Draw(Info);

    if (this->Thickness > 0)
    {
        LRect2F Rect{
            .offset = this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation)
                + this->Padding.GetTopLeftOffset().InStaticPoints(this->GetViewport())
                + LVec2F{0.0f, (this->GetAnchoredSize_v2().y - this->Padding.GetDesiredSizeY().InStaticPoints(this->GetViewport())) / 2.0f - (static_cast<f32>(this->Thickness) / 2.0f)},
            .extent = LVec2F{this->GetAnchoredSize_v2().x, static_cast<f32>(this->Thickness)}
                - LVec2F{this->Padding.GetDesiredSizeX().InStaticPoints(this->GetViewport()), 0.0f}
            };
        Rect.offset.y = maths::round(Rect.offset.y);
        Rect.extent.y = maths::round(Rect.extent.y);
        if (Rect.extent.x > 0.0f && Rect.extent.y > 0.0f)
        {
            Info.AddInstance({.Rect = Rect, .Tint = this->SeparatorTint,});
        }
    }
}
