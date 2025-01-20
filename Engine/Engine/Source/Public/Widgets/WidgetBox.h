// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetNode.h"
#include "Rhi/BoxShaderContext.h"
#include "Widgets/Image.h"
#include "WidgetBox.generated.h"

namespace Jafg
{

struct LBoxBrush
{
    LColor Tint = LColor::White;
    LImage Image = LImage();
};

template <typename TNode>
class TWidgetFactoryWidgetBox : public TWidgetFactory<TNode>
{
public:

    using Super         = TWidgetFactory<TNode>;
    using TFactoryRetTy = typename Super::TFactoryRetTy;

    using Super::operator&;

    TFactoryRetTy& SetBrush(const LBoxBrush& InBrush) { this->This()->SetBrush(InBrush); return this->Self(); }
    TFactoryRetTy& SetTint(const LColor& InTint) { this->This()->SetTint(InTint); return this->Self(); }
    TFactoryRetTy& SetTexture(const LTexture2* InTexture) { this->This()->SetTexture(InTexture); return this->Self(); }
    TFactoryRetTy& SetImage(const LImage& InImage) { this->This()->SetImage(InImage); return this->Self(); }
    TFactoryRetTy& SetPadding(const LPadding& InPadding) { this->This()->SetPadding(InPadding); return this->Self(); }

    TFactoryRetTy& operator&(const LColor& InTint) { return this->SetTint(InTint); }
    TFactoryRetTy& operator&(const LColor&& InTint) { return this->SetTint(InTint); }
    TFactoryRetTy& operator&(const LTexture2* InTexture) { return this->SetTexture(InTexture); }
    TFactoryRetTy& operator&(const LImage& InImage) { return this->SetImage(InImage); }
    TFactoryRetTy& operator&(const LBoxBrush& InBrush) { return this->SetBrush(InBrush); }
    TFactoryRetTy& operator&(const LPadding& InPadding) { return this->SetPadding(InPadding); }
};

//#
//# A widget box is a widget that behaves roughly the same as a region but with
//# the major difference for not allowing children.
//# @see Widgets/WidgetRegion.h
//#
DECLARE_JAFG_CLASS()
class ENGINE_API WWidgetBox : public WWidgetNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WWidgetBox)

public:

    using TWidgetFactoryTy = TWidgetFactoryWidgetBox<Derived>;

    virtual void Draw(LViewport& Context) const override;

    virtual void UpdateDesiredSize() const override;

    FORCEINLINE void SetBrush(const LBoxBrush& InBrush) { this->Brush = InBrush; }
    FORCEINLINE bool HasBrush() const { return this->Brush.IsSet(); }
    FORCEINLINE auto GetBrush() const -> const LBoxBrush& { return this->Brush.GetValue(); }
    FORCEINLINE auto GetPadding() const -> const LPadding& { return this->Padding; }
                void SetTint(const LColor& InTint);
                void SetTexture(const LTexture2* InTexture);
                void SetImage(const LImage& InImage);
    FORCEINLINE void SetPadding(const LPadding& InPadding) { this->Padding = InPadding; }

private:

    void CreateNewShaderContext() const;

    LPadding Padding = LPadding();
    TOptional<LBoxBrush> Brush;
    mutable TOptional<LBoxShaderContext> ShaderContext;
};

} /* ~Namespace Jafg */
