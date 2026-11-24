// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"
#include "Core/Tag.h"
#include "Core/TagExt.h"

namespace Jafg
{

//#
//# Tags used for user input actions and contexts.
//# Not networked.
//#
struct LUserInputTag final: TTag<LUserInputTag, u16>
{
    using Super = TTag<LUserInputTag, u16>;
    using Super::Super;

    NODISCARD ENGINE_API static LMasterThreadTagRegistry<LUserInputTag>& GetMutableRegistry() noexcept;
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
SERDE_ENUM(EModBits, Identity, Shift, Control, Alt, Super, CapsLock, NumLock)
ENUM_STRUCT_FLAGS(EModBits, EModFlags)

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

    FORCEINLINE bool IsLogical() const noexcept
    {
        return this->Logical >= static_cast<i32>(ELogicalKey::FirstLogicalKey) && this->Logical <= static_cast<i32>(ELogicalKey::LastKey);
    }

    template<typename... TKeys> requires(sizeof...(TKeys) > 0
        && (... && (std::same_as<std::remove_cvref_t<TKeys>, ELogicalKey>)))
    FORCEINLINE bool IsAnyLogicalOf(TKeys... Keys) const noexcept
    {
        check(((Keys >= ELogicalKey::FirstLogicalKey) && (Keys <= ELogicalKey::LastKey)) && ...)
        return ((this->Logical == static_cast<i32>(Keys)) || ...);
    }

    FORCEINLINE constexpr bool operator==(LPhysicalKey const& Other) const noexcept
    {
        return this->Scancode == Other.Scancode && this->Logical == Other.Logical;
    }

    LString ToString() const noexcept
    {
        return algo::sprintf("{{Scancode: {}, Logical: {}}}", this->Scancode, this->Logical);
    }
};

//# The modifiers that a key chord can require. Caps lock and num lock are toggles and never part of a chord.
inline constexpr EModFlags ChordModifierMask{EModBits::Shift|EModBits::Control|EModBits::Alt|EModBits::Super};

//#
//# A key together with the modifiers that must be held for it to count (e.g., Ctrl+S).
//# A chord without modifiers matches regardless of the held modifiers. A chord with modifiers matches only while all
//# of them are held, and it takes precedence over a chord without modifiers on the same key.
//#
struct LKeyChord final
{
    FORCEINLINE constexpr LKeyChord() noexcept = default;
    FORCEINLINE constexpr LKeyChord(LPhysicalKey Key, EModFlags Mods = {}) noexcept : Key{Key}, Mods{Mods & ChordModifierMask} {}

    LPhysicalKey Key;
    EModFlags Mods;

    NODISCARD FORCEINLINE constexpr bool HasModifiers() const noexcept { return !!this->Mods; }
    NODISCARD FORCEINLINE constexpr bool AreModifiersHeld(EModFlags Held) const noexcept { return (this->Mods & Held) == this->Mods; }

    FORCEINLINE constexpr bool operator==(LKeyChord const& Other) const noexcept = default;
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
