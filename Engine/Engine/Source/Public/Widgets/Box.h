// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Node.h"
#include "Rhi/BoxShaderContext.h"
#include "Widgets/Image.h"
#include "Box.generated.h"

namespace Jafg
{

struct LBoxBrush
{
    LColor Tint = LColor::White;
    LImage Image = LImage();
};

template <typename TNode>
class TWidgetFactoryBox : public TWidgetFactory<TNode>
{
public:

    using Super         = TWidgetFactory<TNode>;
    using TFactoryRetTy = typename Super::TFactoryRetTy;

    FORCEINLINE TFactoryRetTy& Brush(const LBoxBrush& InBrush) { this->This()->SetBrush(InBrush); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Tint(const LColor& InTint) { this->This()->SetTint(InTint); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Texture(const LTexture2* InTexture) { this->This()->SetTexture(InTexture); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Image(const LImage& InImage) { this->This()->SetImage(InImage); return this->Self(); }
    FORCEINLINE TFactoryRetTy& SetPadding(const LPadding& InPadding) { this->This()->SetPadding(InPadding); return this->Self(); }
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
