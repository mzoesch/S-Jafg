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

DECLARE_JAFG_CLASS()
class ENGINE_API WTextBlock final : public WWidgetNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WTextBlock)

public:

    virtual void Construct() override;
    virtual void Draw(LViewport& Context) const override;

    virtual auto UpdateDesiredSize() const -> void override;

    FORCEINLINE void SetContent(const LSimpleString& InContent) { this->Content = InContent; }
    FORCEINLINE auto SetContent(LSimpleString&& InContent) -> WTextBlock& { this->Content = std::move(InContent); return *this; }
    FORCEINLINE auto GetContent() const -> const LSimpleString& { return this->Content;      }

    FORCEINLINE auto SetColor(const LColor& InColor) -> WTextBlock& { this->Brush.Color = InColor; return *this; }
    FORCEINLINE auto SetMargin(const LMargin& InMargin) -> WTextBlock& { *this->GetSlot()->Margin = InMargin; return *this; }

    FORCEINLINE auto SetBrush(const LTextBlockBrush& InBrush) -> WTextBlock& { this->Brush = InBrush; return *this; }
    FORCEINLINE auto GetBrush() const -> const LTextBlockBrush& { return this->Brush;    }

    FORCEINLINE auto SetPadding(const LPadding& InPadding) -> WTextBlock& { this->Padding = InPadding; return *this; }
    FORCEINLINE auto GetPadding() const -> const LPadding& { return this->Padding; }

    ///////////////////////////////////////////////////////////////////////////////
    // Wsdsml
    ///////////////////////////////////////////////////////////////////////////////

    FORCEINLINE auto operator&(const LColor&     InColor) -> WTextBlock& { return this->SetColor(InColor); }
    FORCEINLINE auto operator&(const LColor&&    InColor) -> WTextBlock& { return this->SetColor(InColor); }
    FORCEINLINE auto operator&(LSimpleString&& InContent) -> WTextBlock& { return this->SetContent(std::forward<LSimpleString>(InContent)); }
    FORCEINLINE auto operator&(LTextBlockBrush&& InBrush) -> WTextBlock& { return this->SetBrush(InBrush); }

    FORCEINLINE auto operator>(LNullptrTy) -> WTextBlock&
    {
        return *this;
    }

    template <typename TNode> FORCEINLINE TNode& operator>>(TNode*& OutNode) { OutNode = this; return *this; }
    template <typename TNode> FORCEINLINE TNode& operator>>(TNode& OutNode)  { OutNode = this; return *this; }

    ///////////////////////////////////////////////////////////////////////////////
    // ~Wsdsml
    ///////////////////////////////////////////////////////////////////////////////

private:

    /**
     * Called only once when the first instance of a WTextBlock is being constructed to load
     * all glyphs from the font file.
     */
    void FirstTimeLoadCharacters();

    /** The padding area between the slot and the content it contains. */
    LPadding Padding = LPadding(4.5f);

    LSimpleString   Content = nullptr;
    LTextBlockBrush Brush   = { };

    uint32  Vao               = 0x0u;
    uint32  Vbo               = 0x0u;
    mutable LBoxShaderContext TintShaderContext = { };
};

} /* ~Namespace Jafg */
