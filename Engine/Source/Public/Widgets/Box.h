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

    FORCEINLINE constexpr LColor const& GetTint() const noexcept { return this->Brush.Tint; }
    FORCEINLINE constexpr LTexture2Ref const& GetTexture() const noexcept { return this->Brush.Texture; }
    FORCEINLINE constexpr f32 GetImageScale() const noexcept { return this->Brush.TextureScale; }
    FORCEINLINE constexpr ETexCoordBehavior     GetImageBehavior() const noexcept { return this->Brush.TexCoordBehavior; }
    FORCEINLINE constexpr vk::SamplerAddressMode GetSamplerAddressMode() const noexcept { return this->Brush.SamplerAddressMode; }
    FORCEINLINE constexpr f32                  GetImagePadding() const noexcept { return this->Brush.ImagePadding; }
    FORCEINLINE constexpr const LVec4F&       GetOutlineRadii() const noexcept { return this->Brush.Radii; }
    FORCEINLINE constexpr f32                  GetOutlineThickness() const noexcept { return this->Brush.OutlineThickness; }
    FORCEINLINE constexpr const LColor&   GetOutlineTint() const noexcept { return this->Brush.OutlineTint; }
    FORCEINLINE constexpr const LPadding&      GetPadding() const noexcept { return this->Brush.Padding; }

    FORCEINLINE constexpr void SetTint(LColor const& InTint) noexcept { this->Brush.Tint = InTint; }
    FORCEINLINE constexpr void SetTexture(LTexture2Ref InTexture) noexcept { this->Brush.Texture = std::move(InTexture); }
    FORCEINLINE constexpr void SetImageScale(f32 InTextureScale) noexcept { this->Brush.TextureScale = InTextureScale; }
    FORCEINLINE constexpr void SetImageBehavior(const ETexCoordBehavior InBehavior) noexcept { this->Brush.TexCoordBehavior = InBehavior; }
    FORCEINLINE constexpr void SetSamplerAddressMode(vk::SamplerAddressMode InSamplerAddressMode) noexcept { this->Brush.SamplerAddressMode = InSamplerAddressMode; }
    FORCEINLINE constexpr void SetImagePadding(const f32 InPadding) noexcept { this->Brush.ImagePadding = InPadding; }
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

    FORCEINLINE decltype(auto) Brush(this auto&& Self, LBoxBrush const& InBrush)
    {
        NODE_FACTORY_SELF().SetBrush(InBrush);
        return NODE_FACTORY_RESULT();
    }
    FORCEINLINE decltype(auto) Tint(this auto&& Self, const LColor& InTint)
    {
        NODE_FACTORY_SELF().SetTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    FORCEINLINE decltype(auto) Texture(this auto&& Self, LTexture2Ref InTexture)
    {
        NODE_FACTORY_SELF().SetTexture(std::move(InTexture));
        return NODE_FACTORY_RESULT();
    }
    FORCEINLINE decltype(auto) ImageScale(this auto&& Self, const f32 InImageScale)
    {
        NODE_FACTORY_SELF().SetImageScale(InImageScale);
        return NODE_FACTORY_RESULT();
    }
    FORCEINLINE decltype(auto) ImageBehavior(this auto&& Self, const ETexCoordBehavior InImageBehavior)
    {
        NODE_FACTORY_SELF().SetTexCoordBehavior(InImageBehavior);
        return NODE_FACTORY_RESULT();
    }
    FORCEINLINE decltype(auto) SamplerAddressMode(this auto&& Self, vk::SamplerAddressMode InSamplerAddressMode)
    {
        NODE_FACTORY_SELF().SetSamplerAddressMode(InSamplerAddressMode);
        return NODE_FACTORY_RESULT();
    }
    FORCEINLINE decltype(auto) ImagePadding(this auto&& Self, const f32 InImagePadding)
    {
        NODE_FACTORY_SELF().SetImagePadding(InImagePadding);
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
    FORCEINLINE decltype(auto) OutlineThickness(this auto&& Self, const f32 InOutlineThickness)
    {
        NODE_FACTORY_SELF().SetOutlineThickness(InOutlineThickness);
        return NODE_FACTORY_RESULT();
    }
    FORCEINLINE decltype(auto) OutlineTint(this auto&& Self, const LColor& InOutlineTint)
    {
        NODE_FACTORY_SELF().SetOutlineTint(InOutlineTint);
        return NODE_FACTORY_RESULT();
    }
    FORCEINLINE decltype(auto) Padding(this auto&& Self, const LPadding& InPadding)
    {
        NODE_FACTORY_SELF().SetPadding(InPadding);
        return NODE_FACTORY_RESULT();
    }

};

} /* ~Namespace Jafg */
