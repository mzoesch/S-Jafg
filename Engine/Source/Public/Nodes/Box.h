// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Node.h"
#include "Nodes/Region.h"
#include "Box.generated.h"

namespace Jafg
{

struct LFactoryBox;

struct LBoxBrush : public LRegionBrush
{
    LPadding Padding;
};

//# A box has the same drawing capabilities as a region, but it does not allow having children.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryBox)
class ENGINE_API WBox : public WNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WBox)

public:

    virtual void Draw(LNodeRenderInfo const& Info) const override;
    virtual void UpdateDesiredSize() const override
    {
        Super::UpdateDesiredSize();
        this->SetDesiredSize(this->Brush.Padding.GetDesiredSize());
    }

    LBoxBrush Brush;
};

struct LFactoryBox : NODE_FACTORY_PARENT(WBox)
{
    NODE_FACTORY_BODY(WBox)

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
    decltype(auto) SkipBrushDraw(this auto&& Self, bool bSkip) noexcept
    {
        NODE_FACTORY_SELF().Brush.bSkipBrushDraw = bSkip;
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
    decltype(auto) Padding(this auto&& Self, LPadding const& Padding)
    {
        NODE_FACTORY_SELF().Brush.Padding = Padding;
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
