// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Overlay.h"
#include "Rhi/Texture2.h"
#include "Rhi/BindlessTextureArray.h"
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
    UBO::BindlessTextureArray::Sampler SamplerAddressMode{ UBO::BindlessTextureArray::Sampler::LinearClampToEdgeSamplerIdx };

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

    ENGINE_API void Draw(LNodeRenderInfo const& Info, LRect2F const& Rect) const noexcept;
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

    LRegionBrush Brush;
};

struct LFactoryRegion : NODE_FACTORY_PARENT(WRegion)
{
    NODE_FACTORY_BODY(WRegion)

    decltype(auto) Brush(this auto&& Self, LRegionBrush const& Brush) noexcept
    {
        NODE_FACTORY_SELF().Brush = Brush;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Tint(this auto&& Self, LColor const& Tint) noexcept
    {
        NODE_FACTORY_SELF().Brush.Tint = Tint;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Texture(this auto&& Self, LTexture2Ref Texture) noexcept
    {
        NODE_FACTORY_SELF().Brush.Texture = std::move(Texture);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) TextureScale(this auto&& Self, f32 Scale) noexcept
    {
        NODE_FACTORY_SELF().Brush.TextureScale = Scale;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) TexCoordBehavior(this auto&& Self, const ETexCoordBehavior Behavior) noexcept
    {
        NODE_FACTORY_SELF().Brush.TexCoordBehavior = Behavior;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SamplerAddressMode(this auto&& Self, UBO::BindlessTextureArray::Sampler AddressMode) noexcept
    {
        NODE_FACTORY_SELF().Brush.SamplerAddressMode = AddressMode;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) TexturePadding(this auto&& Self, f32 Padding) noexcept
    {
        NODE_FACTORY_SELF().Brush.TexturePadding = Padding;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) BackgroundTint(this auto&& Self, LColor const& BackgroundTint) noexcept
    {
        NODE_FACTORY_SELF().Brush.BackgroundTint = BackgroundTint;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Radii(this auto&& Self, LVec4F const& Radii) noexcept
    {
        NODE_FACTORY_SELF().Brush.Radii = Radii;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) ClampRadii(this auto&& Self, bool bClamp) noexcept
    {
        NODE_FACTORY_SELF().Brush.bClampRadii = bClamp;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SkipBrushDraw(this auto&& Self, bool bInSkip) noexcept
    {
        NODE_FACTORY_SELF().Brush.bSkipBrushDraw = bInSkip;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OutlineThickness(this auto&& Self, f32 Thickness) noexcept
    {
        NODE_FACTORY_SELF().Brush.OutlineThickness = Thickness;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OutlineTint(this auto&& Self, LColor const& Tint) noexcept
    {
        NODE_FACTORY_SELF().Brush.OutlineTint = Tint;
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
