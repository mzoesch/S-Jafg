// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Overlay.h"
#include "Rhi/Texture2.h"
#include "Region.generated.h"

namespace Jafg
{

//# High-level image behavior.
enum struct ETexCoordBehavior
{
    //# Scale UVs normalized.
    Scale,
    //# Scale UVs so that the vertical component of the texture is always [0,1] while preserving aspect.
    FitV,
    //# Scale UVs so that the horizontal component of the texture is always [0,1] while preserving aspect.
    FitH,
};

namespace TextureBehavior
{

inline LVec4F FitV(LVec4F UVs, LVec2F NodeSize, LVec2F TargetSize) noexcept
{
    f32 CenterU{(UVs.x + UVs.z) * 0.5f};
    f32 ScaledU{(UVs.w - UVs.y) * ((TargetSize.x  / TargetSize.y) / (NodeSize.x / NodeSize.y))};
    return {
        CenterU - ScaledU * 0.5f, UVs.y,
        CenterU + ScaledU * 0.5f, UVs.w
        };
}

inline LVec4F FitH(LVec4F UVs, LVec2F NodeSize, LVec2F TargetSize) noexcept
{
    f32 CenterV{(UVs.y + UVs.w) * 0.5f};
    f32 ScaledV{(UVs.z - UVs.x) * ((NodeSize.x / NodeSize.y) / (TargetSize.x  / TargetSize.y))};
    return {
        UVs.x, CenterV - ScaledV * 0.5f,
        UVs.z, CenterV + ScaledV * 0.5f
        };
}

} /* ~Namespace TextureBehavior */

struct LRegionBrush
{
    //# The tint of the draw area from this region.
    LColor Tint{ Colors::White };

    //# An optional texture to use as a background.
    LTexture2Ref Texture;

    //# The scale of the image.
    f32 TextureScale{ 1.0 };

    //# How the texture's UV should behave.
    ETexCoordBehavior TexCoordBehavior{ ETexCoordBehavior::Scale };

    //# Texture UV out-of-bounds behavior.
    vk::SamplerAddressMode SamplerAddressMode{ vk::SamplerAddressMode::eClampToBorder };

    //# How much padding to apply to the image.
    f32 ImagePadding{};

    //# Tint of the background if any.
    LColor BackgroundTint{ Colors::Black };

    //# The radii to use for the edges. TL => TR => BR => BL.
    LVec4F Radii{ maths::zero_vector<LVec4F> };
    //# Whether to clamp radii based on their size.
    bool bClampRadii{ true };

    //# The thickness of the outline.
    f32 OutlineThickness{};

    //# The outline color to use.
    LColor OutlineTint{ Colors::White };
};

struct LFactoryRegion;

//#
//# WRegion is an overlay node that can be customized with a #LRegionBrush.
//# A region might still draw outside these bounds.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryRegion)
class ENGINE_API WRegion : public WOverlay
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WRegion)

public:

    virtual void Draw(LNodeRenderInfo const& Info) const override;

    constexpr void SetBrush(LRegionBrush const& InBrush) noexcept { this->Brush = InBrush; }
    constexpr LRegionBrush& GetMutableBrush() noexcept { return this->Brush; }
    constexpr LRegionBrush const& GetBrush() const noexcept { return this->Brush; }

    FORCEINLINE void SetTint(LColor const& InTint) noexcept { this->Brush.Tint = InTint; }
    FORCEINLINE void SetTexture(LTexture2Ref InTexture) noexcept { this->Brush.Texture = std::move(InTexture); }
    FORCEINLINE void SetImageScale(f32 InTextureScale) noexcept { this->Brush.TextureScale = InTextureScale; }
    FORCEINLINE void SetTexCoordBehavior(ETexCoordBehavior InTexCoordBehavior) noexcept { this->Brush.TexCoordBehavior = InTexCoordBehavior; }
    FORCEINLINE void SetSamplerAddressMode(vk::SamplerAddressMode InSamplerAddressMode) noexcept { this->Brush.SamplerAddressMode = InSamplerAddressMode; }
    FORCEINLINE void SetImagePadding(f32 InPadding) noexcept { this->Brush.ImagePadding = InPadding; }
    FORCEINLINE void SetBackgroundTint(LColor const& InBackgroundTint) noexcept { this->Brush.BackgroundTint = InBackgroundTint; }
    FORCEINLINE void SetOutlineThickness(f32 InOutlineThickness) noexcept { this->Brush.OutlineThickness = InOutlineThickness; }
    FORCEINLINE void SetRadii(LVec4F const& InOutlineRadii) noexcept { this->Brush.Radii = InOutlineRadii; }
    FORCEINLINE void SetClampRadii(bool bClamp) noexcept { this->Brush.bClampRadii = bClamp; }
    FORCEINLINE void SetOutlineTint(LColor const& InOutlineTint) noexcept { this->Brush.OutlineTint = InOutlineTint; }

private:

    LRegionBrush Brush;
};

struct LFactoryRegion : NODE_FACTORY_PARENT(WRegion)
{
    NODE_FACTORY_BODY(WRegion)

    decltype(auto) Brush(this auto&& Self, LRegionBrush const& InBrush) noexcept
    {
        NODE_FACTORY_SELF().SetBrush(InBrush);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Tint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Texture(this auto&& Self, LTexture2Ref InTexture) noexcept
    {
        NODE_FACTORY_SELF().SetTexture(std::move(InTexture));
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) ImageScale(this auto&& Self, const f32 InScale) noexcept
    {
        NODE_FACTORY_SELF().SetImageScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) TexCoordBehavior(this auto&& Self, const ETexCoordBehavior InTexCoordBehavior) noexcept
    {
        NODE_FACTORY_SELF().SetTexCoordBehavior(InTexCoordBehavior);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SamplerAddressMode(this auto&& Self, vk::SamplerAddressMode InSamplerAddressMode) noexcept
    {
        NODE_FACTORY_SELF().SetSamplerAddressMode(InSamplerAddressMode);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) ImagePadding(this auto&& Self, const f32 InPadding) noexcept
    {
        NODE_FACTORY_SELF().SetImagePadding(InPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) BackgroundTint(this auto&& Self, LColor const& InBackgroundTint) noexcept
    {
        NODE_FACTORY_SELF().SetBackgroundTint(InBackgroundTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OutlineThickness(this auto&& Self, const f32 InOutlineThickness) noexcept
    {
        NODE_FACTORY_SELF().SetOutlineThickness(InOutlineThickness);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Radii(this auto&& Self, LVec4F const& InOutlineRadii) noexcept
    {
        NODE_FACTORY_SELF().SetRadii(InOutlineRadii);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) ClampRadii(this auto&& Self, bool bClamp) noexcept
    {
        NODE_FACTORY_SELF().SetClampRadii(bClamp);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OutlineTint(this auto&& Self, LColor const& InOutlineTint) noexcept
    {
        NODE_FACTORY_SELF().SetOutlineTint(InOutlineTint);
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
