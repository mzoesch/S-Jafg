// Copyright mzoesch. All rights reserved.

#include "Nodes/Region.h"
#include "Engine/Engine.h"
#include "Rhi/VisualInstance.h"
#include "Rhi/NodeRenderInfo.h"
#include "Rhi/BindlessTextureArray.h"

void Jafg::WRegion::Draw(LNodeRenderInfo const& Info) const
{
    this->DrawRegionBrush(Info);
    Super::Draw(Info);
}

void Jafg::WRegion::DrawRegionBrush(LNodeRenderInfo const& Info) const
{
    auto AnchoredSize{this->GetAnchoredSize_v2()};
    if (this->Brush.bSkipBrushDraw || (AnchoredSize.x <= 0.0f || AnchoredSize.y <= 0.0f))
    {
        return;
    }

    u32 TextureIndex{UBO::BindlessTextureArray::IdentityMulIdx};
    if (this->Brush.Texture.get())
    {
        if (this->Brush.Texture->IsBindless() == false)
        {
            this->GetFrontend().Vk_AddTextureToGlobalBindlessArray(&*this->Brush.Texture);
            check(this->Brush.Texture->IsBindless())
        }

        TextureIndex = this->Brush.Texture->GetBindlessIndex();
    }

    LVec4F TexCoordRect{0.0f, 0.0f, 1.0f, 1.0f};
    if (this->Brush.Texture.get())
    {
        auto Extend{this->Brush.Texture->GetExtentAsVec2F()};
        if (this->Brush.TexCoordBehavior == ETexCoordBehavior::FitV)
        {
            TexCoordRect = MiscUV::FitV(TexCoordRect, Extend, AnchoredSize);
        }
        else if (this->Brush.TexCoordBehavior == ETexCoordBehavior::FitH)
        {
            TexCoordRect = MiscUV::FitH(TexCoordRect, Extend, AnchoredSize);
        }

        TexCoordRect = MiscUV::ApplyPadding(MiscUV::ApplyScale(TexCoordRect, this->Brush.TextureScale), this->Brush.TexturePadding, Extend);
    }

    Info.AddInstance(LVisualInstance{
        .Rect = {this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation), AnchoredSize},
        .Tint = this->Brush.Tint.Bits,
        .BackgroundTint = this->Brush.BackgroundTint.Bits,
        .Radii = this->Brush.bClampRadii
            ? maths::min(this->Brush.Radii, LVec4F{AnchoredSize.x, AnchoredSize.y, AnchoredSize.x, AnchoredSize.y} / 2.0f)
            : this->Brush.Radii,
        .OutlineTint = this->Brush.OutlineTint.Bits,
        .TexCoordRect = TexCoordRect,
        .OutlineThickness = this->Brush.OutlineThickness,
        .TextureIndex = TextureIndex,
        .SamplerIndex = this->Brush.SamplerAddressMode,
        .MsdfPixelRange = 0.0f,
        });

    return;
}
