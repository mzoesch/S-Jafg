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

    ENGINE_API static LTextBlockBrush MakeDefaultSmall();
};

template <typename TNode>
class TWidgetFactoryTextBlock : public TWidgetFactory<TNode>
{
public:

    using Super         = TWidgetFactory<TNode>;
    using TFactoryRetTy = typename Super::TFactoryRetTy;

    using Super::operator&;

    FORCEINLINE TFactoryRetTy& SetContent(const LSimpleString&  InContent) { this->This()->SetContent(InContent); return this->Self(); }
    FORCEINLINE TFactoryRetTy& SetContent(LSimpleString&& InContent) { this->This()->SetContent(std::move(InContent)); return this->Self(); }
    FORCEINLINE TFactoryRetTy& operator& (const LSimpleString&  InContent) { return this->SetContent(InContent); }
    FORCEINLINE TFactoryRetTy& operator& (LSimpleString&& InContent) { return this->SetContent(std::move(InContent)); }

    FORCEINLINE TFactoryRetTy& SetColor(const LColor&  InColor) { this->This()->SetColor(InColor); return this->Self(); }
    FORCEINLINE TFactoryRetTy& SetBrush(const LTextBlockBrush& InBrush) { this->This()->SetBrush(InBrush); return this->Self(); }
    FORCEINLINE TFactoryRetTy& operator& (const LColor&  InColor) { return this->SetColor(InColor); }
    FORCEINLINE TFactoryRetTy& operator& (LTextBlockBrush&  InBrush) { return this->SetBrush(InBrush); }
    FORCEINLINE TFactoryRetTy& operator& (LTextBlockBrush&& InBrush) { return this->SetBrush(InBrush); }

    FORCEINLINE TFactoryRetTy& SetPadding(const LPadding&  InPadding) { this->This()->SetPadding(InPadding); return this->Self(); }
    FORCEINLINE TFactoryRetTy& SetPadding(const LPadding&& InPadding) { this->This()->SetPadding(InPadding); return this->Self(); }
    FORCEINLINE TFactoryRetTy& operator& (const LPadding&  InPadding) { return this->SetPadding(InPadding); }
    FORCEINLINE TFactoryRetTy& operator& (const LPadding&& InPadding) { return this->SetPadding(InPadding); }
};

DECLARE_JAFG_CLASS()
class ENGINE_API WTextBlock final : public WWidgetNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WTextBlock)

public:

    using TWidgetFactoryTy = TWidgetFactoryTextBlock<Derived>;

    virtual void Construct() override;
    virtual void Draw(LViewport& Context) const override;

    virtual auto UpdateDesiredSize() const -> void override;

    FORCEINLINE void EmptyContent() { this->Content.Empty(); }
    FORCEINLINE void SetContent(const LSimpleString& InContent) { this->Content = InContent; }
    FORCEINLINE void SetContent(LSimpleString&& InContent) { this->Content = std::move(InContent); }
    FORCEINLINE auto GetContent() const -> const LSimpleString& { return this->Content;      }

    FORCEINLINE void SetColor(const LColor& InColor) { this->Brush.Color = InColor; }
    FORCEINLINE auto GetColor() const -> const LColor& { return this->Brush.Color; }

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

    LSimpleString   Content = nullptr;
    LTextBlockBrush Brush   = { };

    uint32  Vao               = 0x0u;
    uint32  Vbo               = 0x0u;
    mutable LBoxShaderContext TintShaderContext = { };
};

} /* ~Namespace Jafg */
