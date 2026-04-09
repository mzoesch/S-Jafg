// Copyright mzoesch. All rights reserved.

#include "Nodes/TextButton.h"
#include "Framework/Frontend.h"
#include "Rhi/NodeRenderInfo.h"

void Jafg::WTextButton::Draw(LNodeRenderInfo const& Info) const
{
    Super::Draw(Info);

    if (this->Icon.get() && this->IconBrush.Scale > 0.0f)
    {
        if (this->Icon->IsBindless() == false)
        {
            this->GetFrontend().Vk_AddTextureToGlobalBindlessArray(&*this->Icon);
            check(this->Icon->IsBindless())
        }

        f32 TotalHeight{(this->GetAnchoredSize_v2().y - this->GetPadding().GetDesiredSizeInSpt(this->GetViewport()).y)};
        LVec2F TopLeft{
              this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Viewport)
            + this->GetPadding().GetTopLeftOffsetInSpt(this->GetViewport())
            + LVec2F{0.0f, (TotalHeight - static_cast<f32>(this->Icon->GetExtent().Height * this->IconBrush.Scale)) * 0.5f}
            };
        Info.VisualInstances.emplace_back(LVisualInstance{
            .Rect = {maths::round(TopLeft), this->Icon->GetExtentAsVec2F() * static_cast<f32>(this->IconBrush.Scale)},
            .Tint = this->IconBrush.Tint.Bits,
            .BackgroundTint = Colors::Black.Bits,
            .Radii = maths::zero_vector<LVec4F>,
            .OutlineTint = Colors::Transparent.Bits,
            .TexCoordRect = {0.0f, 0.0f, 1.0f, 1.0f},
            .OutlineThickness = 0.0f,
            .TextureIndex = this->Icon->GetBindlessIndex(),
            .SamplerIndex = UBO::BindlessTextureArray::LinearClampToEdgeSamplerIdx,
            .MsdfPixelRange = 0.0f,
            });
    }

    return;
}

void Jafg::WTextButton::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    if (this->Icon.get() && this->IconBrush.Scale > 0.0f)
    {
        f32 Width{static_cast<f32>(this->Icon->GetExtent().Width * this->IconBrush.Scale)};
        LVec2F Size{Width + InSpt(this->GetViewport(), this->IconBrush.RightPadding), 0.0f};
        this->SetDesiredSizeInSpt(this->GetDesiredSize_v2() + Size);
        this->SetTextDrawOffset(Size);
    }

    return;
}
