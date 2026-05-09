// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Overlay.h"
#include "Rhi/Texture2.h"
#include "Rhi/BindlessTextureArray.h"
#include "Region.generated.h"

namespace Jafg
{

struct LFactoryRegion;

namespace MiscUV
{


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
    rhi::tex_coord_behavior TexCoordBehavior{ rhi::tex_coord_behavior::Scale };

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
    decltype(auto) TexCoordBehavior(this auto&& Self, const rhi::tex_coord_behavior Behavior) noexcept
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
