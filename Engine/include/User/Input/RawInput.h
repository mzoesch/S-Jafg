// Copyright mzoesch. All rights reserved.

#pragma once

#include "User/Input/InputTypes.h"

namespace Jafg
{

enum struct ERawInputStateBits
{
    Identity = 0x00,

    //# The key was just pressed inbetween this and the last frame.
    Press   = 0x01 << 0,
    //# The key is being held down.
    Hold    = 0x01 << 1,
    //# The key is being repeated by the platform's key repeat system.
    Repeat  = 0x01 << 2,
    //# The key was just released inbetween this and the last frame.
    Release = 0x01 << 3,
};
SERDE_ENUM(ERawInputStateBits, Identity, Press, Hold, Repeat, Release)
ENUM_STRUCT_FLAGS(ERawInputStateBits, ERawInputStateFlags)

struct LRawInput final
{
    LPhysicalKey PhysicalKey;
    EModFlags Mods;
    f32 Value{ 1.0f };
    ERawInputStateFlags State;

    /* Do not merge this two with constexpr exprs as this confuses clangds intellisense... */
    template<ERawInputStateFlags State>
    NODISCARD FORCEINLINE bool Is() const noexcept
    {
        return !!(this->State & State);
    }
    //# Takes any combination of (optional) physical and logical keys.
    template<ERawInputStateFlags State, typename... TKeys> requires(sizeof...(TKeys) > 0
        && (... && (std::same_as<std::remove_cvref_t<TKeys>, LPhysicalKey>
             || std::same_as<std::remove_cvref_t<TKeys>, std::optional<LPhysicalKey>>
             || std::same_as<std::remove_cvref_t<TKeys>, ELogicalKey>)))
    NODISCARD FORCEINLINE bool Is(TKeys&&... Keys) const noexcept
    {
        return !!(this->State & State) && ((this->PhysicalKey == [](auto&& Key)
            {
                if constexpr (std::same_as<std::remove_cvref_t<decltype(Key)>, ELogicalKey>)
                {
                    return LPhysicalKey::FromLogical(Key);
                }
                else
                {
                    return std::forward<decltype(Key)>(Key);
                }
            }(std::forward<TKeys>(Keys))) || ...);
    }

    NODISCARD LString ToString() const noexcept
    {
        return algo::sprintf("{{{}: {:.2f} M:{} S:{}}}"
            , this->PhysicalKey.ToString(), this->Value, serde::to_string(this->Mods), serde::to_string(this->State));
    }
};

} /* ~Namespace Jafg */
