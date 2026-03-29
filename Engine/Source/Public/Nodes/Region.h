// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Overlay.h"
#include "Rhi/Texture2.h"
#include "Region.generated.h"

namespace Jafg
{

struct LFactoryRegion;

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
    vk::SamplerAddressMode SamplerAddressMode{ vk::SamplerAddressMode::eClampToEdge };

    //# How much padding to apply to the texture.
    f32 TexturePadding{};

    //# Tint of the background if any.
    LColor BackgroundTint{ Colors::Black };

    //# The radii to use for the edges. TL => TR => BR => BL.
    LVec4F Radii{ maths::zero_vector<LVec4F> };
    //# Whether to clamp radii based on their size.
    bool bClampRadii{ true };

    //# Whether to skip drawing this region.
    bool bSkipBrushDraw{};

    //# The thickness of the outline.
    f32 OutlineThickness{};

    //# The outline color to use.
    LColor OutlineTint{ Colors::White };
};

//# A region is an overlay node that can be customized with a #LRegionBrush.
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

    constexpr void SetTint(LColor const& InTint) noexcept { this->Brush.Tint = InTint; }
    constexpr void SetTexture(LTexture2Ref InTexture) noexcept { this->Brush.Texture = std::move(InTexture); }
    constexpr void SetTextureScale(f32 InScale) noexcept { this->Brush.TextureScale = InScale; }
    constexpr void SetTexCoordBehavior(ETexCoordBehavior InBehavior) noexcept { this->Brush.TexCoordBehavior = InBehavior; }
    constexpr void SetSamplerAddressMode(vk::SamplerAddressMode InAddressMode) noexcept { this->Brush.SamplerAddressMode = InAddressMode; }
    constexpr void SetTexturePadding(f32 InPadding) noexcept { this->Brush.TexturePadding = InPadding; }
    constexpr void SetBackgroundTint(LColor const& InBackgroundTint) noexcept { this->Brush.BackgroundTint = InBackgroundTint; }
    constexpr void SetRadii(LVec4F const& InRadii) noexcept { this->Brush.Radii = InRadii; }
    constexpr void SetClampRadii(bool bInClamp) noexcept { this->Brush.bClampRadii = bInClamp; }
    constexpr void SetSkipBrushDraw(bool bInSkip) noexcept { this->Brush.bSkipBrushDraw = bInSkip; }
    constexpr void SetOutlineThickness(const f32 InThickness) noexcept { this->Brush.OutlineThickness = InThickness; }
    constexpr void SetOutlineTint(LColor const& InTint) noexcept { this->Brush.OutlineTint = InTint; }

    constexpr LColor const& GetTint() const noexcept { return this->Brush.Tint; }
    constexpr LTexture2Ref const& GetTexture() const noexcept { return this->Brush.Texture; }
    constexpr f32 GetTextureScale() const noexcept { return this->Brush.TextureScale; }
    constexpr ETexCoordBehavior GetTexCoordBehavior() const noexcept { return this->Brush.TexCoordBehavior; }
    constexpr vk::SamplerAddressMode GetSamplerAddressMode() const noexcept { return this->Brush.SamplerAddressMode; }
    constexpr f32 GetTexturePadding() const noexcept { return this->Brush.TexturePadding; }
    constexpr LColor const& GetBackgroundTint() const noexcept { return this->Brush.BackgroundTint; }
    constexpr LVec4F const& GetRadii() const noexcept { return this->Brush.Radii; }
    constexpr bool GetClampRadii() const noexcept { return this->Brush.bClampRadii; }
    constexpr bool GetSkipBrushDraw() const noexcept { return this->Brush.bSkipBrushDraw; }
    constexpr f32 GetOutlineThickness() const noexcept { return this->Brush.OutlineThickness; }
    constexpr LColor const& GetOutlineTint() const noexcept { return this->Brush.OutlineTint; }

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
    decltype(auto) TextureScale(this auto&& Self, f32 InScale) noexcept
    {
        NODE_FACTORY_SELF().SetTextureScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) TexCoordBehavior(this auto&& Self, const ETexCoordBehavior InBehavior) noexcept
    {
        NODE_FACTORY_SELF().SetTexCoordBehavior(InBehavior);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SamplerAddressMode(this auto&& Self, vk::SamplerAddressMode InAddressMode) noexcept
    {
        NODE_FACTORY_SELF().SetSamplerAddressMode(InAddressMode);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) TexturePadding(this auto&& Self, f32 InPadding) noexcept
    {
        NODE_FACTORY_SELF().SetTexturePadding(InPadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) BackgroundTint(this auto&& Self, LColor const& InBackgroundTint) noexcept
    {
        NODE_FACTORY_SELF().SetBackgroundTint(InBackgroundTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Radii(this auto&& Self, LVec4F const& InRadii) noexcept
    {
        NODE_FACTORY_SELF().SetRadii(InRadii);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) ClampRadii(this auto&& Self, bool bInClamp) noexcept
    {
        NODE_FACTORY_SELF().SetClampRadii(bInClamp);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SkipBrushDraw(this auto&& Self, bool bInSkip) noexcept
    {
        NODE_FACTORY_SELF().SetSkipBrushDraw(bInSkip);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OutlineThickness(this auto&& Self, f32 InThickness) noexcept
    {
        NODE_FACTORY_SELF().SetOutlineThickness(InThickness);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OutlineTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetOutlineTint(InTint);
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
