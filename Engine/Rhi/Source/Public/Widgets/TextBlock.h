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
    LColor Color;
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
    FORCEINLINE auto GetContent() const -> const LSimpleString& { return this->Content;      }

    FORCEINLINE void SetColor(const LColor& InColor) { this->Brush.Color = InColor; }

    FORCEINLINE void SetBrush(const LTextBlockBrush& InBrush)   { this->Brush = InBrush; }
    FORCEINLINE auto GetBrush() const -> const LTextBlockBrush& { return this->Brush;    }

    ///////////////////////////////////////////////////////////////////////////////
    // Wsdsml
    ///////////////////////////////////////////////////////////////////////////////

    FORCEINLINE auto operator&(const LColor&     InColor) -> WTextBlock& { this->SetColor(InColor);     return *this; }
    FORCEINLINE auto operator&(const LColor&&    InColor) -> WTextBlock& { this->SetColor(InColor);     return *this; }
    FORCEINLINE auto operator&(const char*     InContent) -> WTextBlock& { this->SetContent(InContent); return *this; }
    FORCEINLINE auto operator&(LTextBlockBrush&& InBrush) -> WTextBlock& { this->SetBrush(InBrush);     return *this; }

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

    LSimpleString   Content = "";
    LTextBlockBrush Brush   = { };

    uint32  Vao               = 0x0u;
    uint32  Vbo               = 0x0u;
    Shader* FontShaderProgram = nullptr;
};

} /* ~Namespace Jafg */
