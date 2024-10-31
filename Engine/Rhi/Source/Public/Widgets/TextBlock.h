// Copyright mzoesch. All rights reserved.

#pragma once

#include "WidgetNode.h"
#include "TextBlock.generated.h"

namespace Jafg
{

class Shader;
class LViewport;

struct LTextBlockBrush
{
    LColor Color = LColor::Black;
    float  Scale = 1.0f;
    float X = 0.0f;
    float Y = 0.0f;
};

DECLARE_JAFG_CLASS()
class RHI_API WTextBlock final : public WWidgetNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WTextBlock)

public:

    // WWidgetNode implementation
    virtual void Construct() override;
    virtual void Tick() override;
    virtual void Destruct() override;
    virtual void Draw(LViewport* Context) const override;
    // ~WWidgetNode implementation

    FORCEINLINE void SetContent(const LSimpleString& InContent) { this->Content = InContent; }
    FORCEINLINE void SetContent(LSimpleString&& InContent) { this->Content = std::move(InContent); }
    FORCEINLINE auto GetContent() const -> const LSimpleString& { return this->Content;      }

    FORCEINLINE auto SetColor(const LColor& InColor) -> WTextBlock& { this->Brush.Color = InColor; return *this; }

    FORCEINLINE auto SetBrush(const LTextBlockBrush& InBrush) -> WTextBlock& { this->Brush = InBrush; return *this; }
    FORCEINLINE auto GetBrush() const -> const LTextBlockBrush& { return this->Brush;    }

    ///////////////////////////////////////////////////////////////////////////////
    // Wsdsml
    ///////////////////////////////////////////////////////////////////////////////

    FORCEINLINE auto operator&(const LColor&     InColor) -> WTextBlock& { this->SetColor(InColor);     return *this; }
    FORCEINLINE auto operator&(const LColor&&    InColor) -> WTextBlock& { this->SetColor(InColor);     return *this; }
    FORCEINLINE auto operator&(LSimpleString&& InContent) -> WTextBlock& { this->SetContent(std::forward<LSimpleString>(InContent)); return *this; }
    FORCEINLINE auto operator&(LTextBlockBrush&& InBrush) -> WTextBlock& { this->SetBrush(InBrush);     return *this; }

    FORCEINLINE auto operator>(LNullptrTy) -> WTextBlock&
    {
        return *this;
    }

    template <typename TNode>
    FORCEINLINE auto operator>>(TNode*& OutNode) -> WTextBlock&
    {
        static_assert(std::is_base_of_v<WWidgetNode, TNode>, "TNode must be a subclass of WWidgetNode.");
        OutNode = this;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // ~Wsdsml
    ///////////////////////////////////////////////////////////////////////////////

private:

    /**
     * Called only once when the first instance of a WTextBlock is being constructed to load
     * all glyphs from the font file.
     */
    void FirstTimeLoadCharacters();

    LSimpleString   Content = nullptr;
    LTextBlockBrush Brush   = { };

    uint32  Vao               = 0x0u;
    uint32  Vbo               = 0x0u;
    Shader* FontShaderProgram = nullptr;
};

} /* ~Namespace Jafg */
