// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Overlay.h"
#include "Widgets/RegionForward.h"
#include "Rhi/Image.h"
#include "Region.generated.h"

namespace Jafg
{

template <typename TNode>
class TWidgetFactoryRegion : public TWidgetFactoryParentBase<TNode>
{
public:

    GENERATED_FACTORY_BODY(TWidgetFactoryParentBase)

    FORCEINLINE TFactoryRetTy& Brush(const LRegionBrush& InBrush) { this->This()->SetBrush(InBrush); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Type(const ERegionBrush::Type InType) { this->This()->SetType(InType); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Tint(const LColor& InTint) { this->This()->SetTint(InTint); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Color(const LColor& InColor) { this->This()->SetColor(InColor); return this->Self(); }
    FORCEINLINE TFactoryRetTy& BackgroundColor(const LColor& InBackgroundColor) { this->This()->SetBackgroundColor(InBackgroundColor); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Texture(const LTexture2* InTexture) { this->This()->SetTexture(InTexture); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Image(const LImage& InImage) { this->This()->SetImage(InImage); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OutlineThickness(const f32 InOutlineThickness) { this->This()->SetOutlineThickness(InOutlineThickness); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OutlineRadii(const LVector4& InOutlineRadii) { this->This()->SetOutlineRadii(InOutlineRadii); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OutlineTint(const LColor& InOutlineTint) { this->This()->SetOutlineTint(InOutlineTint); return this->Self(); }
};

//#
//# WRegion is an overlay node that can be customized with a #LRegionBrush.
//# A region might still draw outside these bounds.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactoryRegion)
class ENGINE_API WRegion : public WOverlay
{
    GENERATED_CLASS_BODY()

protected:

    explicit WRegion(const LObjectInitializer& ObjectInitializer);

public:

    virtual void Draw(LViewport& Context) const override;

    void SetBrush(const LRegionBrush& InBrush) { this->Brush = InBrush; }
    void SetBrush(LRegionBrush&& InBrush) { this->Brush = std::move(InBrush); }
    LRegionBrush& GetMutableBrush() { return this->Brush; }
    const LRegionBrush&  GetBrush() const { return this->Brush; }

    FORCEINLINE void SetType(const ERegionBrush::Type InType) { this->Brush.Type = InType; }
    FORCEINLINE void SetTint(const LColor& InTin) { this->Brush.Tint = InTin; }
    FORCEINLINE void SetColor(const LColor& InColor) { this->Brush.Color = InColor; }
    FORCEINLINE void SetBackgroundColor(const LColor& InBackgroundColor) { this->Brush.BackgroundColor = InBackgroundColor; }
    FORCEINLINE void SetTexture(const LTexture2* InTexture) { this->Brush.Image.SetTexture(InTexture); }
    FORCEINLINE void SetImage(const LImage& InImage) { this->Brush.Image = InImage; }
    FORCEINLINE void SetOutlineThickness(const f32 InOutlineThickness) { this->Brush.OutlineThickness = InOutlineThickness; }
    FORCEINLINE void SetOutlineRadii(const LVector4& InOutlineRadii) { this->Brush.Radii = InOutlineRadii; }
    FORCEINLINE void SetOutlineTint(const LColor& InOutlineTint) { this->Brush.OutlineTint = InOutlineTint; }

private:

    LRegionBrush Brush;
};

} /* ~Namespace Jafg */
