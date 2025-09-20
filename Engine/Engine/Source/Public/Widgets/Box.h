// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Node.h"
#include "Rhi/Image.h"
#include "Widgets/BoxForward.h"
#include "Box.generated.h"

namespace Jafg
{

template <typename TNode>
class TWidgetFactoryBox : public TWidgetFactory<TNode>
{
public:

    GENERATED_FACTORY_BODY(TWidgetFactory)

    FORCEINLINE TFactoryRetTy& Type(const ERegionBrush::Type InType) { this->This()->SetType(InType); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Tint(const Lal::LColor& InTint) { this->This()->SetTint(InTint); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Texture(const LTexture2* InTexture) { this->This()->SetTexture(InTexture); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Image(const LImage& InImage) { this->This()->SetImage(InImage); return this->Self(); }
    FORCEINLINE TFactoryRetTy& ImageTint(const Lal::LColor& InImageTint) { this->This()->SetImageTint(InImageTint); return this->Self(); }
    FORCEINLINE TFactoryRetTy& ImageScale(const f32 InImageScale) { this->This()->SetImageScale(InImageScale); return this->Self(); }
    FORCEINLINE TFactoryRetTy& ImageBehavior(const EImageBehavior::Type InImageBehavior) { this->This()->SetImageBehavior(InImageBehavior); return this->Self(); }
    FORCEINLINE TFactoryRetTy& ImageOobm(const EImageOobm::Type InImageOobm) { this->This()->SetImageOobm(InImageOobm); return this->Self(); }
    FORCEINLINE TFactoryRetTy& ImagePadding(const f32 InImagePadding) { this->This()->SetImagePadding(InImagePadding); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OutlineRadii(const LVector4& InOutlineRadii) { this->This()->SetOutlineRadii(InOutlineRadii); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OutlineThickness(const f32 InOutlineThickness) { this->This()->SetOutlineThickness(InOutlineThickness); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OutlineTint(const Lal::LColor& InOutlineTint) { this->This()->SetOutlineTint(InOutlineTint); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Padding(const LPadding& InPadding) { this->This()->SetPadding(InPadding); return this->Self(); }

    FORCEINLINE TFactoryRetTy& Brush(const LBoxBrush& InBrush) { this->This()->SetBrush(InBrush); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Brush(LBoxBrush&& InBrush) { this->This()->SetBrush(std::move(InBrush)); return this->Self(); }
};

//#
//# A widget box is a widget that behaves roughly the same as a region but with
//# the major difference for not allowing children.
//# @see Widgets/WidgetRegion.h
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactoryBox)
class ENGINE_API WBox : public WNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WBox)

public:

    virtual void Draw(LViewport& Context) const override;

    virtual void UpdateDesiredSize() const override;

    FORCEINLINE constexpr void SetBrush(const LBoxBrush& InBrush) noexcept { this->Brush = InBrush; }
    FORCEINLINE constexpr void SetBrush(LBoxBrush&& InBrush) noexcept { this->Brush = std::move(InBrush); }
    FORCEINLINE constexpr LBoxBrush& GetMutableBrush() noexcept { return this->Brush; }
    FORCEINLINE constexpr const LBoxBrush& GetBrush() const noexcept { return this->Brush; }

    FORCEINLINE constexpr ERegionBrush::Type   GetType() const noexcept { return this->Brush.Type; }
    FORCEINLINE constexpr const Lal::LColor&   GetTint() const noexcept { return this->Brush.Tint; }
    FORCEINLINE constexpr const LTexture2*     GetTexture() const noexcept { return this->Brush.Image.GetTexture(); }
    FORCEINLINE constexpr const LImage&        GetImage() const noexcept { return this->Brush.Image; }
    FORCEINLINE constexpr const Lal::LColor&   GetImageTint() const noexcept { return this->Brush.ImageTint; }
    FORCEINLINE constexpr const f32&           GetImageScale() const noexcept { return this->Brush.ImageScale; }
    FORCEINLINE constexpr EImageBehavior::Type GetImageBehavior() const noexcept { return this->Brush.ImageBehavior; }
    FORCEINLINE constexpr EImageOobm::Type     GetImageOobm() const noexcept { return this->Brush.ImageOobm; }
    FORCEINLINE constexpr f32                  GetImagePadding() const noexcept { return this->Brush.ImagePadding; }
    FORCEINLINE constexpr const LVector4&      GetOutlineRadii() const noexcept { return this->Brush.Radii; }
    FORCEINLINE constexpr f32                  GetOutlineThickness() const noexcept { return this->Brush.OutlineThickness; }
    FORCEINLINE constexpr const Lal::LColor&   GetOutlineTint() const noexcept { return this->Brush.OutlineTint; }
    FORCEINLINE constexpr const LPadding&      GetPadding() const noexcept { return this->Brush.Padding; }

    FORCEINLINE constexpr void SetType(const ERegionBrush::Type InType) noexcept { this->Brush.Type = InType; }
    FORCEINLINE constexpr void SetTint(const Lal::LColor& InTin) noexcept { this->Brush.Tint = InTin; }
    FORCEINLINE constexpr void SetTexture(const LTexture2* InTexture) noexcept { this->Brush.Image.SetTexture(InTexture); }
    FORCEINLINE constexpr void SetImage(const LImage& InImage) noexcept { this->Brush.Image = InImage; }
    FORCEINLINE constexpr void SetImageTint(const Lal::LColor& InColor) noexcept { this->Brush.ImageTint = InColor; }
    FORCEINLINE constexpr void SetImageScale(const f32& InScale) noexcept { this->Brush.ImageScale = InScale; }
    FORCEINLINE constexpr void SetImageBehavior(const EImageBehavior::Type InBehavior) noexcept { this->Brush.ImageBehavior = InBehavior; }
    FORCEINLINE constexpr void SetImageOobm(const EImageOobm::Type InOobm) noexcept { this->Brush.ImageOobm = InOobm; }
    FORCEINLINE constexpr void SetImagePadding(const f32 InPadding) noexcept { this->Brush.ImagePadding = InPadding; }
    FORCEINLINE constexpr void SetOutlineRadii(const LVector4& InOutlineRadii) noexcept { this->Brush.Radii = InOutlineRadii; }
    FORCEINLINE constexpr void SetOutlineThickness(const f32 InOutlineThickness) noexcept { this->Brush.OutlineThickness = InOutlineThickness; }
    FORCEINLINE constexpr void SetOutlineTint(const Lal::LColor& InOutlineTint) noexcept { this->Brush.OutlineTint = InOutlineTint; }
    FORCEINLINE constexpr void SetPadding(const LPadding& InPadding) noexcept { this->Brush.Padding = InPadding; }

private:

    LBoxBrush Brush;
};

} /* ~Namespace Jafg */
