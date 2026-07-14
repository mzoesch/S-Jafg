// Copyright mzoesch. All rights reserved.

#include "Nodes/Region.h"
#include "Engine/Engine.h"
#include "Rhi/VisualInstance.h"
#include "Rhi/NodeRenderInfo.h"
#include "Rhi/Bindless.h"
#include "Framework/TextureSubsystem.h"

void Jafg::LRegionBrush::Draw(LNodeRenderInfo const& Info, LRect2F const& Rect) const noexcept
{
    if (this->bSkipBrushDraw || Rect.extent.x <= 0.0f || Rect.extent.y <= 0.0f)
    {
        return;
    }

    auto& TextureSubsystem{*Info.Frontend.GetSubsystemChecked<JTextureSubsystem>()};

    if (std::holds_alternative<LTexture>(this->Background))
    {
        auto& Texture{std::get<LTexture>(this->Background).Texture};
        check(Texture.get())
        if (!Texture->IsBindless())
        {
            TextureSubsystem.AddTextureToGlobalBindlessArray(&*Texture);
            check(Texture->IsBindless())
        }
        Info.AddInstance({
            .Rect = Rect,
            .TexCoordRect = {Texture.get()
                ? rhi::uv::pipe({0.0f, 0.0f, 1.0f, 1.0f}, Texture->GetExtentAsVec2F(), Rect.extent
                    , this->TexCoordBehavior, this->TexturePadding, std::get<LTexture>(this->Background).TextureScale)
                : LVec4F{0.0f, 0.0f, 1.0f, 1.0f}
                },
            .Radii = this->bClampRadii
                ? maths::min(this->Radii, LVec4F{Rect.extent.x, Rect.extent.y, Rect.extent.x, Rect.extent.y} / 2.0f)
                : this->Radii,
            .Tint = this->Tint,
            .BorderTint = this->BorderTint,
            .OutlineTint = this->OutlineTint,
            .OutlineThickness = this->OutlineThickness,
            .TextureIndex = Texture->GetBindlessIndex(),
            .SamplerIndex = std::to_underlying(this->SamplerAddressMode),
            });
    }
    else if (std::holds_alternative<LIcon>(this->Background))
    {
        Info.AddInstance({
            .Rect = {maths::round(Rect.offset), maths::round(Rect.extent)},
            .Radii = this->bClampRadii
                ? maths::min(this->Radii, LVec4F{Rect.extent.x, Rect.extent.y, Rect.extent.x, Rect.extent.y} / 2.0f)
                : this->Radii,
            .Tint = this->BorderTint,
            .BorderTint = this->BorderTint,
            .OutlineTint = this->OutlineTint,
            .OutlineThickness = this->OutlineThickness,
            .SamplerIndex = std::to_underlying(this->SamplerAddressMode),
            });

        if (std::get<LIcon>(this->Background).Scale > 0)
        {
            auto& IconTexture{std::get<LIcon>(this->Background).Texture};
            check(IconTexture.get())
            if (!IconTexture->IsBindless())
            {
                TextureSubsystem.AddTextureToGlobalBindlessArray(&*IconTexture);
                check(IconTexture->IsBindless())
            }
            LVec2F IconExtent{static_cast<f32>(IconTexture->GetExtent().width * std::get<LIcon>(this->Background).Scale)
                    , static_cast<f32>(IconTexture->GetExtent().height * std::get<LIcon>(this->Background).Scale)};
            Info.AddInstance({
                .Rect = {maths::round(Rect.offset + Rect.extent/2.0f - (IconExtent/2.0f)), IconExtent},
                .Tint = this->Tint,
                .BorderTint = this->BorderTint,
                .TextureIndex = IconTexture->GetBindlessIndex(),
                });
        }
    }
    else
    {
        Info.AddInstance({
            .Rect = Rect,
            .Radii = this->bClampRadii
                ? maths::min(this->Radii, LVec4F{Rect.extent.x, Rect.extent.y, Rect.extent.x, Rect.extent.y} / 2.0f)
                : this->Radii,
            .Tint = this->Tint,
            .BorderTint = this->BorderTint,
            .OutlineTint = this->OutlineTint,
            .OutlineThickness = this->OutlineThickness,
            .SamplerIndex = std::to_underlying(this->SamplerAddressMode),
            });
    }

    return;
}

void Jafg::WRegion::Draw(LNodeRenderInfo const& Info) const
{
    this->Brush.Draw(Info, {
        .offset = this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation),
        .extent = this->GetAnchoredSize_v2(),
        });
    Super::Draw(Info);
    return;
}
