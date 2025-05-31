// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Node.h"
#include "Widgets/RegionForward.h"
#include "Rhi/Image.h"
#include "Box.generated.h"

namespace Jafg
{

struct LBoxBrush : public LRegionBrush
{
    LPadding Padding;
};

template <typename TNode>
class TWidgetFactoryBox : public TWidgetFactory<TNode>
{
public:

    GENERATED_FACTORY_BODY(TWidgetFactory)

    FORCEINLINE TFactoryRetTy& Brush(const LBoxBrush& InBrush) { this->This()->SetBrush(InBrush); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Type(const ERegionBrush::Type InType) { this->This()->SetType(InType); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Tint(const LColor& InTint) { this->This()->SetTint(InTint); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Color(const LColor& InColor) { this->This()->SetColor(InColor); return this->Self(); }
    FORCEINLINE TFactoryRetTy& BackgroundColor(const LColor& InBackgroundColor) { this->This()->SetBackgroundColor(InBackgroundColor); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Texture(const LTexture2* InTexture) { this->This()->SetTexture(InTexture); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Image(const LImage& InImage) { this->This()->SetImage(InImage); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OutlineThickness(const f32 InOutlineThickness) { this->This()->SetOutlineThickness(InOutlineThickness); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OutlineRadii(const LVector4& InOutlineRadii) { this->This()->SetOutlineRadii(InOutlineRadii); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OutlineTint(const LColor& InOutlineTint) { this->This()->SetOutlineTint(InOutlineTint); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Padding(const LPadding& InPadding) { this->This()->SetPadding(InPadding); return this->Self(); }
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

    void SetBrush(const LBoxBrush& InBrush) { this->Brush = InBrush; }
    void SetBrush(LBoxBrush&& InBrush) { this->Brush = std::move(InBrush); }
    LBoxBrush& GetMutableBrush() { return this->Brush; }
    const LBoxBrush& GetBrush() const { return this->Brush; }

    FORCEINLINE void SetType(const ERegionBrush::Type InType) { this->Brush.Type = InType; }
    FORCEINLINE void SetTint(const LColor& InTin) { this->Brush.Tint = InTin; }
    FORCEINLINE void SetColor(const LColor& InColor) { this->Brush.Color = InColor; }
    FORCEINLINE void SetBackgroundColor(const LColor& InBackgroundColor) { this->Brush.BackgroundColor = InBackgroundColor; }
    FORCEINLINE void SetTexture(const LTexture2* InTexture) { this->Brush.Image.SetTexture(InTexture); }
    FORCEINLINE void SetImage(const LImage& InImage) { this->Brush.Image = InImage; }
    FORCEINLINE void SetOutlineThickness(const f32 InOutlineThickness) { this->Brush.OutlineThickness = InOutlineThickness; }
    FORCEINLINE void SetOutlineRadii(const LVector4& InOutlineRadii) { this->Brush.Radii = InOutlineRadii; }
    FORCEINLINE void SetOutlineTint(const LColor& InOutlineTint) { this->Brush.OutlineTint = InOutlineTint; }
    FORCEINLINE void SetPadding(const LPadding& InPadding) { this->Brush.Padding = InPadding; }

private:

    LBoxBrush Brush;
};

} /* ~Namespace Jafg */
