// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Overlay.h"
#include "Rhi/BoxShaderContext.h"
#include "Misc/Optional.h"
#include "Widgets/Image.h"
#include "Region.generated.h"

namespace Jafg
{

struct LRegionBrush
{
    LColor Tint { LColor::White };
    LImage Image;
};

template <typename TNode>
class TWidgetFactoryRegion : public TWidgetFactoryParentBase<TNode>
{
public:

    GENERATED_FACTORY_BODY(TWidgetFactoryParentBase)

    FORCEINLINE TFactoryRetTy& Brush(const LRegionBrush& InBrush) { this->This()->SetBrush(InBrush); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Tint(const LColor& InTint) { this->This()->SetTint(InTint); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Texture(const LTexture2* InTexture) { this->This()->SetTexture(InTexture); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Image(const LImage& InImage) { this->This()->SetImage(InImage); return this->Self(); }
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
    bool HasBrush() const { return this->Brush.IsSet(); }
    auto GetBrush() const -> const LRegionBrush& { return this->Brush.GetValue(); }
    void SetTint(const LColor& InTint);
    void SetTexture(const LTexture2* InTexture);
    void SetImage(const LImage& InImage);

private:

    TOptional<LRegionBrush> Brush;
};

} /* ~Namespace Jafg */
