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

    constexpr void SetBrush(LBoxBrush const& InBrush) noexcept { this->Brush = InBrush; }
    constexpr LBoxBrush& GetMutableBrush() noexcept { return this->Brush; }
    constexpr LBoxBrush const& GetBrush() const noexcept { return this->Brush; }

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
    constexpr void SetPadding(LPadding const& InPadding) noexcept { this->Brush.Padding = InPadding; }

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
    constexpr LPadding const& GetPadding() const noexcept { return this->Brush.Padding; }

private:

    LBoxBrush Brush;
};

struct LFactoryBox : NODE_FACTORY_PARENT(WBox)
{
    NODE_FACTORY_BODY(WBox)

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
    decltype(auto) Padding(this auto&& Self, LPadding const& InPadding)
    {
        NODE_FACTORY_SELF().SetPadding(InPadding);
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
