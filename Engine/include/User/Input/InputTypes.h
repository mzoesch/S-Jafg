// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"
#include "Misc/Tag.h"

namespace Jafg
{

struct LUserInputTag : public Jafg::TTag<u16>
{
    using Super = Jafg::TTag<u16>;
    using Super::Super;
    //# Convert the range of characters to a tag.
    ENGINE_API static LUserInputTag ToTag(LStringView S) noexcept;
    //#
    //# Get the tag version of the range of characters.
    //# @return LUserInputTag::NO_TAG if S does not map to a tag.
    //#
    ENGINE_API static LUserInputTag AsTag(LStringView S);
    FORCEINLINE static LUserInputTag AsTagChecked(LStringView S)
    {
        auto Tag{LUserInputTag::AsTag(S)};
        check(Tag.IsSet())
        return Tag;
    }
    ENGINE_API LString ToString() const noexcept;
};

enum struct EModBits
{
    Identity = 0x00,

    Shift       = 0x01 << 0,
    Control     = 0x01 << 1,
    Alt         = 0x01 << 2,
    Super       = 0x01 << 3,
    CapsLock    = 0x01 << 4,
    NumLock     = 0x01 << 5,
};
ENUM_STRUCT_FLAGS(EModBits, EModFlags)
inline LStringView LexToString(EModBits Bit) noexcept
{
    switch (Bit)
    {
    case EModBits::Identity: return "Identity";
    case EModBits::Shift: return "Shift";
    case EModBits::Control: return "Control";
    case EModBits::Alt: return "Alt";
    case EModBits::Super: return "Super";
    case EModBits::CapsLock: return "CapsLock";
    case EModBits::NumLock: return "NumLock";
    }
    std::unreachable();
}
inline LString LexToString(EModFlags Flags) noexcept
{
    std::stringstream Stream;
    if (Flags & EModBits::Shift) { Stream << "Shift|"; }
    if (Flags & EModBits::Control) { Stream << "Control|"; }
    if (Flags & EModBits::Alt) { Stream << "Alt|"; }
    if (Flags & EModBits::Super) { Stream << "Super|"; }
    if (Flags & EModBits::CapsLock) { Stream << "CapsLock|"; }
    if (Flags & EModBits::NumLock) { Stream << "NumLock|"; }

    LString Result{Stream.str()};
    if (Result.empty()) { return "Identity"; }
    return Stream.str();
}

//#
//# Named physical keys. Not all keys might exist on a user's setup.
//# Retrieve their physical representation with #LFrontend::GetPhysicalKey.
//#
enum struct ELogicalKey
{
    #define DETAIL_JAFG_INPUT_TYPES_WRAPPER(Key) Key,
    #include "User/Input/InputTypesDetail.h"
    FirstKey = LeftShift,
    FirstLogicalKey = MouseX,
    LastKey = ThumbMouseButton8,
};
inline LStringView LexToString(ELogicalKey Key) noexcept
{
    switch (Key)
    {
#define DETAIL_JAFG_INPUT_TYPES_WRAPPER(Key) case ELogicalKey::Key: return #Key;
#include "User/Input/InputTypesDetail.h"
    default: break;
    }
    std::unreachable();
}

//#
//# Physical representation of a key.
//# This repr is platform specific but consistent and should not be synced across devices.
//#
struct LPhysicalKey final
{
    i32 Scancode{ INDEX_NONE };
    i32 Logical{ INDEX_NONE };

    FORCEINLINE static constexpr LPhysicalKey FromLogical(ELogicalKey Key) noexcept
    {
        check(Key >= ELogicalKey::FirstLogicalKey && Key <= ELogicalKey::LastKey)
        return LPhysicalKey{.Logical = static_cast<i32>(Key)};
    }
    FORCEINLINE bool IsLogical(ELogicalKey Key) const noexcept
    {
        check(Key >= ELogicalKey::FirstLogicalKey && Key <= ELogicalKey::LastKey)
        return this->Logical == static_cast<i32>(Key);
    }
    FORCEINLINE constexpr bool operator==(LPhysicalKey const& Other) const noexcept
    {
        return this->Scancode == Other.Scancode && this->Logical == Other.Logical;
    }
    ENGINE_API LString ToString() const noexcept
    {
        return algo::sprintf("{{Scancode: {}, Logical: {}}}", this->Scancode, this->Logical);
    }
};

} /* ~Namespace Jafg */

template<>
struct std::formatter<Jafg::LUserInputTag> : std::formatter<LString>
{
    FORCEINLINE std::format_context::iterator format(Jafg::LUserInputTag InTag, std::format_context& InContext) const
    {
        return std::formatter<LString>::format(InTag.ToString(), InContext);
    }
};
