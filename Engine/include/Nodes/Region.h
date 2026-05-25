// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Overlay.h"
#include "Nodes/ButtonBase.h"
#include "Rhi/Texture2.h"
#include "Rhi/Bindless.h"
#include "Region.generated.h"

namespace Jafg
{

struct LFactoryRegion;

struct LRegionBrush
{
    //# The tint of the draw area from this region.
    LColor Tint{ Colors::White };
    //# Tint of the background if sampled.
    LColor BorderTint{ Colors::Black };
    //# The outline color to use.
    LColor OutlineTint{ Colors::White };
    //# The thickness of the outline.
    f32 OutlineThickness{};

    //# An optional texture to use as a background.
    struct LTexture
    {
        LTexture2Ref Texture;
        //# The scale of the texture.
        f32 TextureScale{ 1.0 };
    };
    //# An icon to draw on top.
    struct LIcon
    {
        LTexture2Ref Texture;
        //# Scale of the icon. A scale of zero means that the icon is not rendered. This is a completely valid state.
        u32 Scale{ 1 };
    };
    std::variant<std::monostate, LTexture, LIcon> Background;
    //# How much padding to apply to the texture.
    f32 TexturePadding{};
    //# How the texture's UV should behave.
    rhi::tex_coord_behavior TexCoordBehavior{ rhi::tex_coord_behavior::Scale };

    //# Whether to skip drawing this region.
    bool bSkipBrushDraw{};
    //# Texture UV out-of-bounds behavior.
    UBO::Bindless::Sampler SamplerAddressMode{ UBO::Bindless::Sampler::LinearClampToEdgeSamplerIdx };

    //# Whether to clamp radii based on their size.
    bool bClampRadii{ true };
    //# The radii to use for the edges. TL => TR => BR => BL.
    LVec4F Radii{ maths::zero_vector<LVec4F> };

    ENGINE_API void Draw(LNodeRenderInfo const& Info, LRect2F const& Rect) const noexcept;
    NODISCARD FORCEINLINE static LTexture Texture(LOptionalTexture2Ref const& Texture) noexcept { return LTexture{Texture.GetResolved()}; }
    NODISCARD FORCEINLINE static LIcon Icon(LOptionalTexture2Ref const& Texture) noexcept { return LIcon{Texture.GetResolved()}; }
};

namespace Detail
{

template<typename TBrush> requires std::is_base_of_v<LRegionBrush, TBrush>
struct TButtonBaseStyle<TBrush>
{
    TBrush NormalBrush   {LRegionBrush{.Tint={0x15,0x15,0x15}, .BorderTint={0x15,0x15,0x15}, .OutlineTint=Colors::Black}};
    TBrush HoverBrush    {LRegionBrush{.Tint={0x1C,0x1C,0x1C}, .BorderTint={0x1C,0x1C,0x1C}, .OutlineTint=Colors::White}};
    TBrush PressBrush    {LRegionBrush{.Tint={0x24,0x24,0x24}, .BorderTint={0x24,0x24,0x24}, .OutlineTint=Colors::White}};
    TBrush SelectedBrush {LRegionBrush{.Tint={0x24,0x24,0x24}, .BorderTint={0x24,0x24,0x24}, .OutlineTint=Colors::White}};
    TBrush DisabledBrush {LRegionBrush{.Tint={0x0F,0x0F,0x0F}, .BorderTint={0x0F,0x0F,0x0F}, .OutlineTint=Colors::Black}};
};

} /* ~Namespace Detail */

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
    decltype(auto) BorderTint(this auto&& Self, LColor const& Tint) noexcept
    {
        NODE_FACTORY_SELF().Brush.BorderTint = Tint;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OutlineTint(this auto&& Self, LColor const& Tint) noexcept
    {
        NODE_FACTORY_SELF().Brush.OutlineTint = Tint;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OutlineThickness(this auto&& Self, f32 Thickness) noexcept
    {
        NODE_FACTORY_SELF().Brush.OutlineThickness = Thickness;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Texture(this auto&& Self, LRegionBrush::LTexture Texture) noexcept
    {
        NODE_FACTORY_SELF().Brush.Background = std::move(Texture);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Texture(this auto&& Self, LOptionalTexture2Ref Texture) noexcept
    {
        NODE_FACTORY_SELF().Brush.Background = LRegionBrush::LTexture{Texture.GetResolved()};
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) TextureScale(this auto&& Self, f32 Scale) noexcept
    {
        check(!Self._IsDecommissioned())
        auto& Me{DETAIL_JAFG_NODE_FACTORY_SELF()};
        check(std::holds_alternative<LRegionBrush::LTexture>(Me.Brush.Background))
        std::get<LRegionBrush::LTexture>(Me.Brush.Background).TextureScale = Scale;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Icon(this auto&& Self, LRegionBrush::LTexture Icon) noexcept
    {
        NODE_FACTORY_SELF().Brush.Background = std::move(Icon);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Icon(this auto&& Self, LOptionalTexture2Ref Icon) noexcept
    {
        NODE_FACTORY_SELF().Brush.Background = LRegionBrush::LIcon{Icon.GetResolved()};
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) IconScale(this auto&& Self, u32 Scale) noexcept
    {
        check(!Self._IsDecommissioned())
        auto& Me{DETAIL_JAFG_NODE_FACTORY_SELF()};
        check(std::holds_alternative<LRegionBrush::LIcon>(Me.Brush.Background))
        std::get<LRegionBrush::LIcon>(Me.Brush.Background).Scale = Scale;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) TexturePadding(this auto&& Self, f32 Padding) noexcept
    {
        NODE_FACTORY_SELF().Brush.TexturePadding = Padding;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) TexCoordBehavior(this auto&& Self, const rhi::tex_coord_behavior Behavior) noexcept
    {
        NODE_FACTORY_SELF().Brush.TexCoordBehavior = Behavior;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SkipBrushDraw(this auto&& Self, bool bInSkip) noexcept
    {
        NODE_FACTORY_SELF().Brush.bSkipBrushDraw = bInSkip;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SamplerAddressMode(this auto&& Self, UBO::Bindless::Sampler AddressMode) noexcept
    {
        NODE_FACTORY_SELF().Brush.SamplerAddressMode = AddressMode;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) ClampRadii(this auto&& Self, bool bClamp) noexcept
    {
        NODE_FACTORY_SELF().Brush.bClampRadii = bClamp;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Radii(this auto&& Self, LVec4F const& Radii) noexcept
    {
        NODE_FACTORY_SELF().Brush.Radii = Radii;
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
