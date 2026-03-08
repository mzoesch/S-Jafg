// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Node.h"
#include "Widgets/Region.h"
#include "Box.generated.h"

namespace Jafg
{

struct LFactoryBox;

struct LBoxBrush : public LRegionBrush
{
    LPadding Padding;
};

//#
//# A widget box is a widget that behaves roughly the same as a region but with
//# the major difference for not allowing children.
//# @see Widgets/WidgetRegion.h
//#
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

    constexpr void SetBrush(LBoxBrush const& InBrush) noexcept { this->Brush = InBrush; }
    constexpr LBoxBrush& GetMutableBrush() noexcept { return this->Brush; }
    constexpr LBoxBrush const& GetBrush() const noexcept { return this->Brush; }

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
    FORCEINLINE constexpr void SetPadding(const LPadding& InPadding) noexcept { this->Brush.Padding = InPadding; }

private:

    LBoxBrush Brush;
};

struct LFactoryBox : NODE_FACTORY_PARENT(WBox)
{
    NODE_FACTORY_BODY(WBox)

    decltype(auto) SkipBrushDraw(this auto&& Self, bool bInSkipBrushDraw) noexcept
    {
        NODE_FACTORY_SELF().SetSkipBrushDraw(bInSkipBrushDraw);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Brush(this auto&& Self, LBoxBrush const& InBrush)
    {
        NODE_FACTORY_SELF().SetBrush(InBrush);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Tint(this auto&& Self, const LColor& InTint)
    {
        NODE_FACTORY_SELF().SetTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Texture(this auto&& Self, LTexture2Ref InTexture)
    {
        NODE_FACTORY_SELF().SetTexture(std::move(InTexture));
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) TextureScale(this auto&& Self, const f32 InTextureScale)
    {
        NODE_FACTORY_SELF().SetTextureScale(InTextureScale);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) TexCoordBehavior(this auto&& Self, const ETexCoordBehavior InTexCoordBehavior)
    {
        NODE_FACTORY_SELF().SetTexCoordBehavior(InTexCoordBehavior);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) SamplerAddressMode(this auto&& Self, vk::SamplerAddressMode InSamplerAddressMode)
    {
        NODE_FACTORY_SELF().SetSamplerAddressMode(InSamplerAddressMode);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) TexturePadding(this auto&& Self, const f32 InTexturePadding)
    {
        NODE_FACTORY_SELF().SetTexturePadding(InTexturePadding);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) BackgroundTint(this auto&& Self, LColor const& InBackgroundTint)
    {
        NODE_FACTORY_SELF().SetBackgroundTint(InBackgroundTint);
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
    decltype(auto) OutlineThickness(this auto&& Self, const f32 InOutlineThickness)
    {
        NODE_FACTORY_SELF().SetOutlineThickness(InOutlineThickness);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) OutlineTint(this auto&& Self, const LColor& InOutlineTint)
    {
        NODE_FACTORY_SELF().SetOutlineTint(InOutlineTint);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Padding(this auto&& Self, const LPadding& InPadding)
    {
        NODE_FACTORY_SELF().SetPadding(InPadding);
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
