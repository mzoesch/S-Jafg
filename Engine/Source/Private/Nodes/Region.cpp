// Copyright mzoesch. All rights reserved.

#include "Nodes/Region.h"
#include "Engine/Engine.h"
#include "Rhi/VisualInstance.h"
#include "Rhi/NodeRenderInfo.h"
#include "Rhi/BindlessTextureArray.h"

void Jafg::LRegionBrush::Draw(LNodeRenderInfo const& Info, LRect2F const& Rect) const noexcept
{
    if (this->bSkipBrushDraw || Rect.Extent.x <= 0.0f || Rect.Extent.y <= 0.0f)
    {
        return;
    }

    u32 TextureIndex{UBO::BindlessTextureArray::IdentityMulIdx};
    if (this->Texture.get())
    {
        if (this->Texture->IsBindless() == false)
        {
            Info.Frontend.Vk_AddTextureToGlobalBindlessArray(&* this->Texture);
            check(this->Texture->IsBindless())
        }
        TextureIndex = this->Texture->GetBindlessIndex();
    }

    LVec4F TexCoordRect{0.0f, 0.0f, 1.0f, 1.0f};
    if (this->Texture.get())
    {
        auto Extend{this->Texture->GetExtentAsVec2F()};
        if (this->TexCoordBehavior == ETexCoordBehavior::FitV)
        {
            TexCoordRect = MiscUV::FitV(TexCoordRect, Extend, Rect.Extent);
        }
        else if (this->TexCoordBehavior == ETexCoordBehavior::FitH)
        {
            TexCoordRect = MiscUV::FitH(TexCoordRect, Extend, Rect.Extent);
        }
        TexCoordRect = MiscUV::ApplyPadding(MiscUV::ApplyScale(TexCoordRect, this->TextureScale), this->TexturePadding, Extend);
    }

    Info.AddInstance({
        .Rect = Rect,
        .Tint = this->Tint.Bits,
        .BackgroundTint = this->BackgroundTint.Bits,
        .Radii = this->bClampRadii
            ? maths::min(this->Radii, LVec4F{Rect.Extent.x, Rect.Extent.y, Rect.Extent.x, Rect.Extent.y} / 2.0f)
            : this->Radii,
        .OutlineTint = this->OutlineTint.Bits,
        .TexCoordRect = TexCoordRect,
        .OutlineThickness = this->OutlineThickness,
        .TextureIndex = TextureIndex,
        .SamplerIndex = this->SamplerAddressMode,
        .MsdfPixelRange = 0.0f,
        });

    return;
}

void Jafg::WRegion::Draw(LNodeRenderInfo const& Info) const
{
    this->Brush.Draw(Info, {
        .Offset = this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation),
        .Extent = this->GetAnchoredSize_v2(),
        });
    Super::Draw(Info);
    return;
}
