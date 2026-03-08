// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Overlay.h"
#include "Rhi/Texture2.h"
#include "Region.generated.h"

namespace Jafg
{

//# High-level texture coordinates behavior.
enum struct ETexCoordBehavior
{
    //# Scale UVs normalized.
    Scale,
    //# Scale UVs so that the vertical component of the texture is always [0,1] while preserving aspect.
    FitV,
    //# Scale UVs so that the horizontal component of the texture is always [0,1] while preserving aspect.
    FitH,
};

namespace MiscUV
{

inline constexpr LVec4F FitV(LVec4F UVs, LVec2F TextureExtend, LVec2F TargetSize) noexcept
{
    f32 CenterU{(UVs.x + UVs.z) * 0.5f};
    f32 ScaledU{(UVs.w - UVs.y) * ((TargetSize.x  / TargetSize.y) / (TextureExtend.x / TextureExtend.y))};
    return {
        CenterU - ScaledU * 0.5f, UVs.y,
        CenterU + ScaledU * 0.5f, UVs.w
        };
}

inline constexpr LVec4F FitH(LVec4F UVs, LVec2F TextureExtend, LVec2F TargetSize) noexcept
{
    f32 CenterV{(UVs.y + UVs.w) * 0.5f};
    f32 ScaledV{(UVs.z - UVs.x) * ((TextureExtend.x / TextureExtend.y) / (TargetSize.x  / TargetSize.y))};
    return {
        UVs.x, CenterV - ScaledV * 0.5f,
        UVs.z, CenterV + ScaledV * 0.5f
        };
}

inline constexpr LVec4F ApplyScale(LVec4F const& UVs, f32 Scale) noexcept
{
    check(Scale != 0.0f)
    LVec2F Center{(maths::xy(UVs) + maths::zw(UVs)) * 0.5f};
    LVec2F HalfSize{(maths::zw(UVs) - maths::xy(UVs)) * 0.5f / Scale};
    return {
        Center - HalfSize,
        Center + HalfSize,
        };
}

inline constexpr LVec4F ApplyPadding(LVec4F UVs, f32 Padding, LVec2F const& Extend) noexcept
{
    LVec2F PaddingUV{Padding / Extend.x, Padding / Extend.y};
    return {
        maths::xy(UVs) + PaddingUV,
        maths::zw(UVs) - PaddingUV,
        };
}

} /* ~Namespace MiscUV */

struct LRegionBrush
{
    //# The tint of the draw area from this region.
    LColor Tint{ Colors::White };

    //# An optional texture to use as a background.
    LTexture2Ref Texture;

    //# The scale of the texture.
    f32 TextureScale{ 1.0 };

    //# How the texture's UV should behave.
    ETexCoordBehavior TexCoordBehavior{ ETexCoordBehavior::Scale };

    //# Texture UV out-of-bounds behavior.
    vk::SamplerAddressMode SamplerAddressMode{ vk::SamplerAddressMode::eClampToBorder };

    //# How much padding to apply to the texture.
    f32 TexturePadding{};

    //# Tint of the background if any.
    LColor BackgroundTint{ Colors::Black };

    //# The radii to use for the edges. TL => TR => BR => BL.
    LVec4F Radii{ maths::zero_vector<LVec4F> };
    //# Whether to clamp radii based on their size.
    bool bClampRadii{ true };

    //# Whether to skip drawing this region.
    bool bSkipBrushDraw{ false };

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

    FORCEINLINE void SetSkipBrushDraw(bool bInSkipBrushDraw) noexcept { this->Brush.bSkipBrushDraw = bInSkipBrushDraw; }
    FORCEINLINE constexpr void SetTint(LColor const& InTint) noexcept { this->Brush.Tint = InTint; }
    FORCEINLINE constexpr void SetTexture(LTexture2Ref InTexture) noexcept { this->Brush.Texture = std::move(InTexture); }
    FORCEINLINE constexpr void SetTextureScale(f32 InTextureScale) noexcept { this->Brush.TextureScale = InTextureScale; }
    FORCEINLINE constexpr void SetImageBehavior(const ETexCoordBehavior InBehavior) noexcept { this->Brush.TexCoordBehavior = InBehavior; }
    FORCEINLINE constexpr void SetSamplerAddressMode(vk::SamplerAddressMode InSamplerAddressMode) noexcept { this->Brush.SamplerAddressMode = InSamplerAddressMode; }
    FORCEINLINE constexpr void SetTexturePadding(const f32 InPadding) noexcept { this->Brush.TexturePadding = InPadding; }
    FORCEINLINE constexpr void SetBackgroundTint(LColor const& InBackgroundTint) noexcept { this->Brush.BackgroundTint = InBackgroundTint; }
    FORCEINLINE constexpr void SetRadii(const LVec4F& InOutlineRadii) noexcept { this->Brush.Radii = InOutlineRadii; }
    FORCEINLINE void SetClampRadii(bool bClamp) noexcept { this->Brush.bClampRadii = bClamp; }
    FORCEINLINE constexpr void SetOutlineThickness(const f32 InOutlineThickness) noexcept { this->Brush.OutlineThickness = InOutlineThickness; }
    FORCEINLINE constexpr void SetOutlineTint(const LColor& InOutlineTint) noexcept { this->Brush.OutlineTint = InOutlineTint; }

private:

    LRegionBrush Brush;
};

struct LFactoryRegion : NODE_FACTORY_PARENT(WRegion)
{
    NODE_FACTORY_BODY(WRegion)

    decltype(auto) SkipBrushDraw(this auto&& Self, bool bInSkipBrushDraw) noexcept
    {
        NODE_FACTORY_SELF().SetSkipBrushDraw(bInSkipBrushDraw);
        return NODE_FACTORY_RESULT();
    }
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
    decltype(auto) TextureScale(this auto&& Self, const f32 InScale) noexcept
    {
        NODE_FACTORY_SELF().SetTextureScale(InScale);
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
    decltype(auto) TexturePadding(this auto&& Self, const f32 InPadding) noexcept
    {
        NODE_FACTORY_SELF().SetTexturePadding(InPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) BackgroundTint(this auto&& Self, LColor const& InBackgroundTint)
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
