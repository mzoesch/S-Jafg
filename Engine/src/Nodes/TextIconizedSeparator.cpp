// Copyright mzoesch. All rights reserved.

#include "Nodes/TextIconizedSeparator.h"
#include "Rhi/NodeRenderInfo.h"

void Jafg::WTextIconizedLeftSeparator::Draw(LNodeRenderInfo const& Info) const
{
    Super::Draw(Info);

    if (this->Thickness > 0 && this->GetTextDesiredSize().y > 0.0f)
    {
        if (this->TextBrush.IsTextLeftAligned())
        {
            auto RelativeTextTopLeft{this->GetRelativeTextTopLeft()};
            LRect2F Rect{
                .offset = this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation)
                    + RelativeTextTopLeft + LVec2F{this->GetTextDesiredSize().x, 0.0f}
                    + LVec2F{static_cast<f32>(this->TextOffset), 0.0f}
                    + LVec2F{0.0f, this->GetTextDesiredSize().y / 2.0f - static_cast<f32>(this->Thickness) / 2.0f},
                .extent = LVec2F{this->GetAnchoredSize_v2().x, static_cast<f32>(this->Thickness)}
                    - LVec2F{this->TextBrush.Padding.GetRightOffset().InStaticPoints(this->GetViewport()), 0.0f}
                    - RelativeTextTopLeft - LVec2F{this->GetTextDesiredSize().x, 0.0f}
                    - LVec2F{static_cast<f32>(this->TextOffset), 0.0f}
                    };
            Rect.offset.y = maths::round(Rect.offset.y);
            Rect.extent.y = maths::round(Rect.extent.y);
            if (Rect.extent.x > 0.0f && Rect.extent.y > 0.0f)
            {
                Info.AddInstance({.Rect = Rect, .Tint = this->SeparatorTint,});
            }
        }
        else
        {
            std::unreachable(); // TODO: Implement
        }
    }

    return;
}

void Jafg::WTextIconizedRightSeparator::Draw(LNodeRenderInfo const& Info) const
{
    Super::Draw(Info);

    if (this->Thickness > 0 && this->GetTextDesiredSize().y > 0.0f)
    {
        if (this->TextBrush.IsTextLeftAligned())
        {
            auto RelativeTextTopLeft{this->GetRelativeTextTopLeft()};
            LRect2F Rect{
                .offset = this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation)
                    + RelativeTextTopLeft + LVec2F{this->GetTextDesiredSize().x, 0.0f}
                + LVec2F{static_cast<f32>(this->TextOffset), 0.0f}
                + LVec2F{0.0f, this->GetTextDesiredSize().y / 2.0f - static_cast<f32>(this->Thickness) / 2.0f},
            .extent = LVec2F{this->GetAnchoredSize_v2().x, static_cast<f32>(this->Thickness)}
                - LVec2F{this->TextBrush.Padding.GetRightOffset().InStaticPoints(this->GetViewport()), 0.0f}
                - RelativeTextTopLeft - LVec2F{this->GetTextDesiredSize().x, 0.0f}
                - LVec2F{static_cast<f32>(this->TextOffset), 0.0f}
            };
            Rect.offset.y = maths::round(Rect.offset.y);
            Rect.extent.y = maths::round(Rect.offset.y);
            if (Rect.extent.x > 0.0f && Rect.extent.y > 0.0f)
            {
                Info.AddInstance({.Rect = Rect, .Tint = this->SeparatorTint,});
            }
        }
        else
        {
            std::unreachable(); // TODO: Implement
        }
    }

    return;
}
