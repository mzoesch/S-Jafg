// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Node.h"
#include "Rhi/BoxShaderContext.h"
#include "TextBlock.generated.h"

namespace Jafg
{

class LShader;
class LViewport;
namespace ETextHAlign { enum Type : u8; }
namespace ETextVAlign { enum Type : u8; }

//#
//# Vertical text alignment is not often used, so we just use the horizontal alignment.
//#
namespace ETextAlign = ETextHAlign;
namespace ETextHAlign
{

enum Type : u8
{
    Left,
    Center,
    Right
    // TODO Justify
};

FORCEINLINE bool IsLeft(const Type InAlign)   { return InAlign == Left; }
FORCEINLINE bool IsCenter(const Type InAlign) { return InAlign == Center; }
FORCEINLINE bool IsRight(const Type InAlign)  { return InAlign == Right; }

} /* ~Namespace ETextAlign */

namespace ETextVAlign
{

enum Type : u8
{
    Top,
    Center,
    Bottom
};

FORCEINLINE bool IsTop(const Type InAlign)    { return InAlign == Top; }
FORCEINLINE bool IsCenter(const Type InAlign) { return InAlign == Center; }
FORCEINLINE bool IsBottom(const Type InAlign) { return InAlign == Bottom; }

} /* ~Namespace ETextVAlign */


struct LTextBlockBrush
{
    LColor            Tint  = LColor::Transparent;
    LColor            Color = LColor::White;
    float             Scale = 1.0f;
    ETextHAlign::Type HAlign = ETextHAlign::Left;
    ETextVAlign::Type VAlign = ETextVAlign::Top;

    FORCEINLINE LTextBlockBrush& TintRet(const LColor& InTint) { this->Tint = InTint; return *this; }
    FORCEINLINE LTextBlockBrush& ColorRet(const LColor& InColor) { this->Color = InColor; return *this; }
    FORCEINLINE LTextBlockBrush& ScaleRet(const float InScale) { this->Scale = InScale; return *this; }

    FORCEINLINE bool IsLeftAligned()    const { return ETextHAlign::IsLeft(this->HAlign);   }
    FORCEINLINE bool IsCenterAligned()  const { return ETextHAlign::IsCenter(this->HAlign); }
    FORCEINLINE bool IsHCenterAligned() const { return ETextHAlign::IsCenter(this->HAlign); }
    FORCEINLINE bool IsRightAligned()   const { return ETextHAlign::IsRight(this->HAlign);  }

    FORCEINLINE bool IsTopAligned()     const { return ETextVAlign::IsTop(this->VAlign);    }
    FORCEINLINE bool IsVCenterAligned() const { return ETextVAlign::IsCenter(this->VAlign); }
    FORCEINLINE bool IsBottomAligned()  const { return ETextVAlign::IsBottom(this->VAlign); }

    ENGINE_API static LTextBlockBrush Header();
    ENGINE_API static LTextBlockBrush SubHeader();
    ENGINE_API static LTextBlockBrush Body();
    ENGINE_API static LTextBlockBrush Compact();
    ENGINE_API static LTextBlockBrush Small();
    ENGINE_API static LTextBlockBrush Tiny();
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
    FORCEINLINE TFactoryRetTy& Align(const ETextHAlign::Type InAlign) { this->This()->SetHAlign(InAlign); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Align(const ETextVAlign::Type InAlign) { this->This()->SetVAlign(InAlign); return this->Self(); }
    FORCEINLINE TFactoryRetTy& HAlign(const ETextHAlign::Type InAlign) { this->This()->SetHAlign(InAlign); return this->Self(); }
    FORCEINLINE TFactoryRetTy& VAlign(const ETextVAlign::Type InAlign) { this->This()->SetVAlign(InAlign); return this->Self(); }

    FORCEINLINE TFactoryRetTy& Padding(const LPadding&  InPadding) { this->This()->SetPadding(InPadding); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Padding(const LPadding&& InPadding) { this->This()->SetPadding(InPadding); return this->Self(); }
};

DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactoryTextBlock)
class ENGINE_API WTextBlock final : public WNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WTextBlock)

public:

    virtual void Construct() override;
    virtual void Draw(LViewport& Context) const override;

    virtual void UpdateDesiredSize() const override;

    FORCEINLINE void EmptyContent() { this->Content.Empty(); }
    FORCEINLINE void SetContent(const LString& InContent) { this->Content = InContent; }
    FORCEINLINE void SetContent(LString&& InContent) { this->Content = std::move(InContent); }
    FORCEINLINE auto GetContent() const -> const LString& { return this->Content; }

    FORCEINLINE void SetColor(const LColor& InColor) { this->Brush.Color = InColor; }
    FORCEINLINE auto GetColor() const -> const LColor& { return this->Brush.Color; }
    FORCEINLINE void SetTint(const LColor& InTint) { this->Brush.Tint = InTint; }
    FORCEINLINE auto GetTint() const -> const LColor& { return this->Brush.Tint; }
    FORCEINLINE void SetScale(const float InScale) { this->Brush.Scale = InScale; }
    FORCEINLINE auto GetScale() const -> float { return this->Brush.Scale; }
    FORCEINLINE void SetAlign(const ETextHAlign::Type InAlign) { this->Brush.HAlign = InAlign; }
    FORCEINLINE void SetAlign(const ETextVAlign::Type InAlign) { this->Brush.VAlign = InAlign; }
    FORCEINLINE auto GetAlign() const -> ETextHAlign::Type { return this->Brush.HAlign; }
    FORCEINLINE void SetHAlign(const ETextHAlign::Type InAlign) { this->Brush.HAlign = InAlign; }
    FORCEINLINE auto GetHAlign() const -> ETextHAlign::Type { return this->Brush.HAlign; }
    FORCEINLINE void SetVAlign(const ETextVAlign::Type InAlign) { this->Brush.VAlign = InAlign; }
    FORCEINLINE auto GetVAlign() const -> ETextVAlign::Type { return this->Brush.VAlign; }

    FORCEINLINE void SetBrush(const LTextBlockBrush& InBrush) { this->Brush = InBrush; }
    FORCEINLINE auto GetBrush() const -> const LTextBlockBrush& { return this->Brush;    }

    FORCEINLINE void SetPadding(const LPadding& InPadding) { this->Padding = InPadding; }
    FORCEINLINE auto GetPadding() const -> const LPadding& { return this->Padding; }

    FORCEINLINE LVector2 GetDesiredSizeOfRawText() const { return this->TextDesiredSize; }

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

    mutable LVector2 TextDesiredSize = LVector2::Zero();

    u32 Vao { 0x0u };
    u32 Vbo { 0x0u };
};

} /* ~Namespace Jafg */
