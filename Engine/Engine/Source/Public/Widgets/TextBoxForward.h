// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/BoxForward.h"
#include "User/UserPreferencesForward.h"

namespace Jafg
{

class WNode;
class WTextBox;
struct LTextBoxBrush;

namespace ETextHAlign { enum Type : u8; }
namespace ETextVAlign { enum Type : u8; }

//# Vertical text alignment is not often used, so we just use the horizontal alignment.
namespace ETextAlign = ETextHAlign;

typedef TFunction<void(LString const& InNewContent)> LTextBoxChangedDelegate;

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

namespace ETextScale
{

enum Type : u8
{
    Header,
    SubHeader,
    Body,
    Compact,
    Small,
    Tiny
};

} /* ~Namespace ETextScale */

struct LTextScale final
{
    FORCEINLINE constexpr LTextScale() noexcept
#if LAL_PLATFORM_USES_LITTLE_ENDIAN
        : PredefinedScale(static_cast<u8>(ETextScale::Body))
        , bPredefined(true)
#else /* LAL_PLATFORM_USES_LITTLE_ENDIAN */
        : bPredefined(true)
        , PredefinedScale(static_cast<u8>(ETextScale::Body))
#endif /* !LAL_PLATFORM_USES_LITTLE_ENDIAN */
    {
        check( this->bPredefined == true )
        check( this->GetPredefinedScale() == ETextScale::Body )
    }

    FORCEINLINE constexpr LTextScale(LTextScale const& Other) noexcept { this->Assign(Other); }
    FORCEINLINE constexpr LTextScale& operator=(LTextScale const& Other) noexcept { this->Assign(Other); return *this; }

    FORCEINLINE LTextScale(f32 CustomScale) noexcept
        : bPredefined(false)
    {
        check( CustomScale > 0.0f )
        this->CustomScale = std::bit_cast<u32>(CustomScale);
        check( this->bPredefined == false )
        check( this->GetCustomScale() == CustomScale )
    }

    FORCEINLINE LTextScale(ETextScale::Type Predefined) noexcept
#if LAL_PLATFORM_USES_LITTLE_ENDIAN
        : PredefinedScale(static_cast<u8>(Predefined))
        , bPredefined(true)
#else /* LAL_PLATFORM_USES_LITTLE_ENDIAN */
        : bPredefined(true)
        , PredefinedScale(static_cast<u8>(Predefined))
#endif /* !LAL_PLATFORM_USES_LITTLE_ENDIAN */
    {
        check( this->bPredefined == true )
        check( this->GetPredefinedScale() == Predefined )
    }

    FORCEINLINE LTextScale& operator=(f32 CustomScale) noexcept
    {
        this->bPredefined = false;
        check( CustomScale > 0.0f )
        this->CustomScale = std::bit_cast<u32>(CustomScale);

        check( this->bPredefined == false )
        check( this->GetCustomScale() == CustomScale )

        return *this;
    }

    FORCEINLINE LTextScale& operator=(ETextScale::Type Predefined) noexcept
    {
        this->bPredefined = true;
        this->PredefinedScale = static_cast<u8>(Predefined);

        check( this->bPredefined == true )
        check( this->GetPredefinedScale() == Predefined )

        return *this;
    }

    FORCEINLINE constexpr bool IsCustom() const noexcept { return this->bPredefined == false; }
    FORCEINLINE constexpr bool IsPredefined() const noexcept { return this->bPredefined; }

    FORCEINLINE constexpr ETextScale::Type GetPredefinedScale() const noexcept
    {
        check( this->bPredefined == true )
        return static_cast<ETextScale::Type>(this->PredefinedScale);
    }

    FORCEINLINE constexpr f32 GetCustomScale() const noexcept
    {
        check( this->bPredefined == false )
        return std::bit_cast<f32>(this->CustomScale);
    }

    ENGINE_API f32 InSpt(LViewport const& Viewport) const noexcept;
    ENGINE_API f32 InSpt(WNode const& Node) const noexcept;

    FORCEINLINE void Assign(LTextScale const& Other) noexcept
    {
        this->bPredefined = Other.bPredefined;

        if (this->bPredefined)
        {
            this->PredefinedScale = Other.PredefinedScale;

            check( this->bPredefined == true )
            check( this->GetPredefinedScale() == Other.GetPredefinedScale() )
        }
        else
        {
            this->CustomScale = Other.CustomScale;

            check( this->bPredefined == false )
            check( this->GetCustomScale() == Other.GetCustomScale() )
        }

        return;
    }

    FORCEINLINE constexpr static LTextScale Header() noexcept { return LTextScale{ETextScale::Header}; }
    FORCEINLINE constexpr static LTextScale SubHeader() noexcept { return LTextScale{ETextScale::SubHeader}; }
    FORCEINLINE constexpr static LTextScale Body() noexcept { return LTextScale{ETextScale::Body}; }
    FORCEINLINE constexpr static LTextScale Compact() noexcept { return LTextScale{ETextScale::Compact}; }
    FORCEINLINE constexpr static LTextScale Small() noexcept { return LTextScale{ETextScale::Small}; }
    FORCEINLINE constexpr static LTextScale Tiny() noexcept { return LTextScale{ETextScale::Tiny}; }

private:

    union
    {
#if LAL_PLATFORM_USES_LITTLE_ENDIAN
        struct
        {
            u8 PredefinedScale;
            bool _padding1;
            bool _padding2;
            bool _padding3 : 7;
            bool bPredefined : 1;
        };
#else /* LAL_PLATFORM_USES_LITTLE_ENDIAN */
        struct
        {
            bool bPredefined : 1;
            bool _padding3 : 7;
            bool _padding2;
            bool _padding1;
            u8 PredefinedScale;
        };
#endif /* !LAL_PLATFORM_USES_LITTLE_ENDIAN */
        u32 CustomScale;
    };
};
static_assert(sizeof(LTextScale) == sizeof(u32));

struct LTextBoxBrush : public LBoxBrush
{
    Lal::LColor TextColor{ Lal::LColor::White };
    LTextScale TextScale;
    ETextHAlign::Type TextHAlign{ ETextHAlign::Left };
    ETextVAlign::Type TextVAlign{ ETextVAlign::Top };
    bool bRespectContentHeight{ false };

    FORCEINLINE constexpr LTextBoxBrush& TextColorRet(const Lal::LColor& InColor) noexcept { this->TextColor = InColor; return *this; }
    FORCEINLINE constexpr LTextBoxBrush& TextScaleRet(const ETextScale::Type InScale) noexcept { this->TextScale = InScale; return *this; }
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

    FORCEINLINE constexpr static LTextBoxBrush Default() noexcept
    {
        LTextBoxBrush Brush;
        Brush.Tint = Lal::LColor::Transparent;
        Brush.Padding = { 4, 2 };
        return Brush;
    }

    FORCEINLINE constexpr static LTextBoxBrush Header() noexcept { return LTextBoxBrush::Default().TextScaleRet(ETextScale::Header); }
    FORCEINLINE constexpr static LTextBoxBrush SubHeader() noexcept { return LTextBoxBrush::Default().TextScaleRet(ETextScale::SubHeader); }
    FORCEINLINE constexpr static LTextBoxBrush Body() noexcept { return LTextBoxBrush::Default().TextScaleRet(ETextScale::Body); }
    FORCEINLINE constexpr static LTextBoxBrush Compact() noexcept { return LTextBoxBrush::Default().TextScaleRet(ETextScale::Compact); }
    FORCEINLINE constexpr static LTextBoxBrush Small() noexcept { return LTextBoxBrush::Default().TextScaleRet(ETextScale::Small); }
    FORCEINLINE constexpr static LTextBoxBrush Tiny() noexcept { return LTextBoxBrush::Default().TextScaleRet(ETextScale::Tiny); }
};

} /* ~Namespace Jafg */
