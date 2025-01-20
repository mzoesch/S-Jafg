// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetParent.h"
#include "Rhi/BoxShaderContext.h"
#include "Misc/Optional.h"
#include "Widgets/Image.h"
#include "WidgetRegion.generated.h"

namespace Jafg
{

struct LRegionBrush
{
    LColor Tint  = LColor::White;
    LImage Image = LImage();
};

template <typename TNode>
class TWidgetFactoryWidgetRegion : public TWidgetFactoryParentBase<TNode>
{
public:

    using Super         = TWidgetFactoryParentBase<TNode>;
    using TFactoryRetTy = typename Super::TFactoryRetTy;

    using Super::operator&;

    FORCEINLINE TFactoryRetTy& SetBrush(const LRegionBrush& InBrush) { this->This()->SetBrush(InBrush); return this->Self(); }
    FORCEINLINE TFactoryRetTy& SetTint(const LColor& InTint) { this->This()->SetTint(InTint); return this->Self(); }
    FORCEINLINE TFactoryRetTy& SetTexture(const LTexture2* InTexture) { this->This()->SetTexture(InTexture); return this->Self(); }
    FORCEINLINE TFactoryRetTy& SetImage(const LImage& InImage) { this->This()->SetImage(InImage); return this->Self(); }

    FORCEINLINE TFactoryRetTy& operator&(const LColor&  InTint) { return this->SetTint(InTint); }
    FORCEINLINE TFactoryRetTy& operator&(const LColor&& InTint) { return this->SetTint(InTint); }
    FORCEINLINE TFactoryRetTy& operator&(const LTexture2* InTexture) { return this->SetTexture(InTexture); }
    FORCEINLINE TFactoryRetTy& operator&(const LImage&  InImage) { return this->SetImage(InImage); }
    FORCEINLINE TFactoryRetTy& operator&(const LRegionBrush& InBrush) { return this->SetBrush(InBrush); }
};

//#
//# WWidgetRegion is a resizable Parent node. Every region has its layout bounds defined as (0, 0, width, height).
//# A region might still draw outside these bounds.
//#
DECLARE_JAFG_CLASS()
class ENGINE_API WWidgetRegion : public WWidgetParent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WWidgetRegion)

public:

    using TWidgetFactoryTy = TWidgetFactoryWidgetRegion<Derived>;

    virtual void Draw(LViewport& Context) const override;
    virtual void UpdateDesiredSize() const override;

    void SetBrush(const LRegionBrush& InBrush) { this->Brush = InBrush; }
    bool HasBrush() const { return this->Brush.IsSet(); }
    auto GetBrush() const -> const LRegionBrush& { return this->Brush.GetValue(); }
    void SetTint(const LColor& InTint);
    void SetTexture(const LTexture2* InTexture);
    void SetImage(const LImage& InImage);

private:

    void CreateNewShaderContext() const;

    TOptional<LRegionBrush> Brush;
    mutable TOptional<LBoxShaderContext> ShaderContext;
};

} /* ~Namespace Jafg */
