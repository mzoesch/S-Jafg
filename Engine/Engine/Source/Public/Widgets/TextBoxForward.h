// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/BoxForward.h"

namespace Jafg
{

class WTextBox;
struct LTextBoxBrush;

namespace ETextHAlign { enum Type : u8; }
namespace ETextVAlign { enum Type : u8; }

//# Vertical text alignment is not often used, so we just use the horizontal alignment.
namespace ETextAlign = ETextHAlign;

MAKE_DELEGATE_SIGNATURE(LTextBoxChangedDelegate, void, const LString& InNewContent)

namespace ETextHAlign
{

enum Type : u8
{
    Left,
    Center,
    Right
    // TODO: Justify
};

FORCEINLINE constexpr bool IsLeft(const Type InAlign)   noexcept { return InAlign == Left; }
FORCEINLINE constexpr bool IsCenter(const Type InAlign) noexcept { return InAlign == Center; }
FORCEINLINE constexpr bool IsRight(const Type InAlign)  noexcept { return InAlign == Right; }

} /* ~Namespace ETextAlign */

namespace ETextVAlign
{

enum Type : u8
{
    Top,
    Center,
    Bottom
};

FORCEINLINE constexpr bool IsTop(const Type InAlign)    noexcept { return InAlign == Top; }
FORCEINLINE constexpr bool IsCenter(const Type InAlign) noexcept { return InAlign == Center; }
FORCEINLINE constexpr bool IsBottom(const Type InAlign) noexcept { return InAlign == Bottom; }

} /* ~Namespace ETextVAlign */

struct LTextBoxBrush : public LBoxBrush
{
    Lal::LColor TextColor { Lal::LColor::White };
    f32 TextScale { 1.0f };
    ETextHAlign::Type TextHAlign { ETextHAlign::Left };
    ETextVAlign::Type TextVAlign { ETextVAlign::Top };
    bool bRespectContentHeight { false };

    FORCEINLINE constexpr LTextBoxBrush& TextColorRet(const Lal::LColor& InColor) noexcept { this->TextColor = InColor; return *this; }
    FORCEINLINE constexpr LTextBoxBrush& TextScaleRet(const f32 InScale) noexcept { this->TextScale = InScale; return *this; }
    FORCEINLINE constexpr LTextBoxBrush& TextHAlignRet(const ETextHAlign::Type InAlign) noexcept { this->TextHAlign = InAlign; return *this; }
    FORCEINLINE constexpr LTextBoxBrush& TextVAlignRet(const ETextVAlign::Type InAlign) noexcept { this->TextVAlign = InAlign; return *this; }
    FORCEINLINE constexpr LTextBoxBrush& RespectContentHeightRet(const bool bInRespect) noexcept { this->bRespectContentHeight = bInRespect; return *this; }

    FORCEINLINE constexpr bool IsLeftAligned()    const noexcept { return ETextHAlign::IsLeft(this->TextHAlign);   }
    FORCEINLINE constexpr bool IsCenterAligned()  const noexcept { return ETextHAlign::IsCenter(this->TextHAlign); }
    FORCEINLINE constexpr bool IsHCenterAligned() const noexcept { return ETextHAlign::IsCenter(this->TextHAlign); }
    FORCEINLINE constexpr bool IsRightAligned()   const noexcept { return ETextHAlign::IsRight(this->TextHAlign);  }

    FORCEINLINE constexpr bool IsTopAligned()     const noexcept { return ETextVAlign::IsTop(this->TextVAlign);    }
    FORCEINLINE constexpr bool IsVCenterAligned() const noexcept { return ETextVAlign::IsCenter(this->TextVAlign); }
    FORCEINLINE constexpr bool IsBottomAligned()  const noexcept { return ETextVAlign::IsBottom(this->TextVAlign); }

    ENGINE_API static LTextBoxBrush Header();
    ENGINE_API static LTextBoxBrush SubHeader();
    ENGINE_API static LTextBoxBrush Body();
    ENGINE_API static LTextBoxBrush Compact();
    ENGINE_API static LTextBoxBrush Small();
    ENGINE_API static LTextBoxBrush Tiny();
};

} /* ~Namespace Jafg */
