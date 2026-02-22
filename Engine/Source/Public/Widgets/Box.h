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

    virtual void Draw(LViewport& Context) const override;

    virtual void UpdateDesiredSize() const override;

    FORCEINLINE CONSTEXPR_CHECK void SetBrush(LBoxBrush const& InBrush) noexcept { this->Brush = InBrush; }
    FORCEINLINE constexpr LBoxBrush& GetMutableBrush() noexcept { return this->Brush; }
    FORCEINLINE constexpr LBoxBrush const& GetBrush() const noexcept { return this->Brush; }

    FORCEINLINE constexpr ERegionBrush   GetType() const noexcept { return this->Brush.Type; }
    FORCEINLINE constexpr const LColor&   GetTint() const noexcept { return this->Brush.Tint; }
    // FORCEINLINE constexpr const LTexture2*     GetTexture() const noexcept { return this->Brush.Image.GetTexture(); }
    // FORCEINLINE constexpr const LImage&        GetImage() const noexcept { return this->Brush.Image; }
    FORCEINLINE constexpr const LColor&   GetImageTint() const noexcept { return this->Brush.ImageTint; }
    FORCEINLINE constexpr f32           GetImageScale() const noexcept { return this->Brush.ImageScale; }
    FORCEINLINE constexpr EImageBehavior     GetImageBehavior() const noexcept { return this->Brush.ImageBehavior; }
    FORCEINLINE constexpr EImageOobm         GetImageOobm() const noexcept { return this->Brush.ImageOobm; }
    FORCEINLINE constexpr f32                  GetImagePadding() const noexcept { return this->Brush.ImagePadding; }
    FORCEINLINE constexpr const LVec4F&       GetOutlineRadii() const noexcept { return this->Brush.Radii; }
    FORCEINLINE constexpr f32                  GetOutlineThickness() const noexcept { return this->Brush.OutlineThickness; }
    FORCEINLINE constexpr const LColor&   GetOutlineTint() const noexcept { return this->Brush.OutlineTint; }
    FORCEINLINE constexpr const LPadding&      GetPadding() const noexcept { return this->Brush.Padding; }

    FORCEINLINE constexpr void SetType(const ERegionBrush InType) noexcept { this->Brush.Type = InType; }
    FORCEINLINE constexpr void SetTint(const LColor& InTin) noexcept { this->Brush.Tint = InTin; }
    // FORCEINLINE constexpr void SetTexture(const LTexture2* InTexture) noexcept { this->Brush.Image.SetTexture(InTexture); }
    // FORCEINLINE constexpr void SetImage(const LImage& InImage) noexcept { this->Brush.Image = InImage; }
    FORCEINLINE constexpr void SetImageTint(const LColor& InColor) noexcept { this->Brush.ImageTint = InColor; }
    FORCEINLINE constexpr void SetImageScale(const f32& InScale) noexcept { this->Brush.ImageScale = InScale; }
    FORCEINLINE constexpr void SetImageBehavior(const EImageBehavior InBehavior) noexcept { this->Brush.ImageBehavior = InBehavior; }
    FORCEINLINE constexpr void SetImageOobm(const EImageOobm InOobm) noexcept { this->Brush.ImageOobm = InOobm; }
    FORCEINLINE constexpr void SetImagePadding(const f32 InPadding) noexcept { this->Brush.ImagePadding = InPadding; }
    FORCEINLINE constexpr void SetOutlineRadii(const LVec4F& InOutlineRadii) noexcept { this->Brush.Radii = InOutlineRadii; }
    FORCEINLINE constexpr void SetOutlineThickness(const f32 InOutlineThickness) noexcept { this->Brush.OutlineThickness = InOutlineThickness; }
    FORCEINLINE constexpr void SetOutlineTint(const LColor& InOutlineTint) noexcept { this->Brush.OutlineTint = InOutlineTint; }
    FORCEINLINE constexpr void SetPadding(const LPadding& InPadding) noexcept { this->Brush.Padding = InPadding; }

private:

    LBoxBrush Brush;
};

struct LFactoryBox : NODE_FACTORY_PARENT(WBox)
{
    NODE_FACTORY_BODY(WBox)

    FORCEINLINE decltype(auto) Type(this auto&& Self, const ERegionBrush InType)
    {
        NODE_FACTORY_SELF().SetType(InType);
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
    FORCEINLINE decltype(auto) ImageTint(this auto&& Self, const LColor& InImageTint)
    {
        NODE_FACTORY_SELF().SetImageTint(InImageTint);
        return NODE_FACTORY_RESULT();
    }
    FORCEINLINE decltype(auto) ImageScale(this auto&& Self, const f32 InImageScale)
    {
        NODE_FACTORY_SELF().SetImageScale(InImageScale);
        return NODE_FACTORY_RESULT();
    }
    FORCEINLINE decltype(auto) ImageBehavior(this auto&& Self, const EImageBehavior InImageBehavior)
    {
        NODE_FACTORY_SELF().SetImageBehavior(InImageBehavior);
        return NODE_FACTORY_RESULT();
    }
    FORCEINLINE decltype(auto) ImageOobm(this auto&& Self, const EImageOobm InImageOobm)
    {
        NODE_FACTORY_SELF().SetImageOobm(InImageOobm);
        return NODE_FACTORY_RESULT();
    }
    FORCEINLINE decltype(auto) ImagePadding(this auto&& Self, const f32 InImagePadding)
    {
        NODE_FACTORY_SELF().SetImagePadding(InImagePadding);
        return NODE_FACTORY_RESULT();
    }
    FORCEINLINE decltype(auto) OutlineRadii(this auto&& Self, const LVec4F& InOutlineRadii)
    {
        NODE_FACTORY_SELF().SetOutlineRadii(InOutlineRadii);
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
    FORCEINLINE decltype(auto) Brush(this auto&& Self, const LBoxBrush& InBrush)
    {
        NODE_FACTORY_SELF().SetBrush(InBrush);
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
