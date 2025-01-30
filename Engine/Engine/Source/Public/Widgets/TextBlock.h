// Copyright mzoesch. All rights reserved.

#pragma once

#include "WidgetNode.h"
#include "Rhi/BoxShaderContext.h"
#include "TextBlock.generated.h"

namespace Jafg
{

class LShader;
class LViewport;

struct LTextBlockBrush
{
    LColor Tint  = LColor::Transparent;
    LColor Color = LColor::White;
    float  Scale = 1.0f;

    FORCEINLINE LTextBlockBrush& TintRet(const LColor& InTint) { this->Tint = InTint; return *this; }
    FORCEINLINE LTextBlockBrush& ColorRet(const LColor& InColor) { this->Color = InColor; return *this; }
    FORCEINLINE LTextBlockBrush& ScaleRet(const float InScale) { this->Scale = InScale; return *this; }

    ENGINE_API static LTextBlockBrush Small();
};

template <typename TNode>
class TWidgetFactoryTextBlock : public TWidgetFactory<TNode>
{
public:

    using Super         = TWidgetFactory<TNode>;
    using TFactoryRetTy = typename Super::TFactoryRetTy;

    FORCEINLINE TFactoryRetTy& Content(const LString&  InContent) { this->This()->SetContent(InContent); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Content(LString&& InContent) { this->This()->SetContent(std::move(InContent)); return this->Self(); }

    FORCEINLINE TFactoryRetTy& Color(const LColor&  InColor) { this->This()->SetColor(InColor); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Tint(const LColor&  InTint) { this->This()->SetTint(InTint); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Scale(const float InScale) { this->This()->SetScale(InScale); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Brush(const LTextBlockBrush& InBrush) { this->This()->SetBrush(InBrush); return this->Self(); }

    FORCEINLINE TFactoryRetTy& Padding(const LPadding&  InPadding) { this->This()->SetPadding(InPadding); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Padding(const LPadding&& InPadding) { this->This()->SetPadding(InPadding); return this->Self(); }
};

DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactoryTextBlock)
class ENGINE_API WTextBlock final : public WWidgetNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WTextBlock)

public:

    virtual void Construct() override;
    virtual void Draw(LViewport& Context) const override;

    virtual auto UpdateDesiredSize() const -> void override;

    FORCEINLINE void EmptyContent() { this->Content.Empty(); }
    FORCEINLINE void SetContent(const LString& InContent) { this->Content = InContent; }
    FORCEINLINE void SetContent(LString&& InContent) { this->Content = std::move(InContent); }
    FORCEINLINE auto GetContent() const -> const LString& { return this->Content;      }

    FORCEINLINE void SetColor(const LColor& InColor) { this->Brush.Color = InColor; }
    FORCEINLINE auto GetColor() const -> const LColor& { return this->Brush.Color; }
    FORCEINLINE void SetTint(const LColor& InTint) { this->Brush.Tint = InTint; }
    FORCEINLINE auto GetTint() const -> const LColor& { return this->Brush.Tint; }
    FORCEINLINE void SetScale(const float InScale) { this->Brush.Scale = InScale; }
    FORCEINLINE auto GetScale() const -> float { return this->Brush.Scale; }

    FORCEINLINE void SetBrush(const LTextBlockBrush& InBrush) { this->Brush = InBrush; }
    FORCEINLINE auto GetBrush() const -> const LTextBlockBrush& { return this->Brush;    }

    FORCEINLINE void SetMargin(const LMargin& InMargin) { *this->GetSlot()->Margin = InMargin; }
    FORCEINLINE auto GetMargin() const -> const LMargin* { return this->GetSlot()->Margin; }
    FORCEINLINE void SetPadding(const LPadding& InPadding) { this->Padding = InPadding; }
    FORCEINLINE auto GetPadding() const -> const LPadding& { return this->Padding; }

private:

    //#
    //# Called only once when the first instance of a WTextBlock is being constructed to load
    //# all glyphs from the font file.
    //#
    void FirstTimeLoadCharacters();

    //# The padding area between the slot and the content it contains.
    LPadding Padding = LPadding(4.5f);

    LString         Content = nullptr;
    LTextBlockBrush Brush   = { };

    uint32  Vao               = 0x0u;
    uint32  Vbo               = 0x0u;
    mutable LBoxShaderContext TintShaderContext = { };
};

} /* ~Namespace Jafg */
