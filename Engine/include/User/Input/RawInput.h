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
ENUM_STRUCT_FLAGS(ERawInputStateBits, ERawInputStateFlags)
inline LStringView LexToString(ERawInputStateBits Bits) noexcept
{
    switch (Bits)
    {
    case ERawInputStateBits::Identity: return "Identity";
    case ERawInputStateBits::Press:    return "Press";
    case ERawInputStateBits::Hold:     return "Hold";
    case ERawInputStateBits::Repeat:   return "Repeat";
    case ERawInputStateBits::Release:  return "Release";
    }
    std::unreachable();
}
inline LString LexToString(ERawInputStateFlags Flags) noexcept
{
    std::stringstream Stream;
    if (Flags & ERawInputStateBits::Press)   { Stream << "Press|"; }
    if (Flags & ERawInputStateBits::Hold)    { Stream << "Hold|"; }
    if (Flags & ERawInputStateBits::Repeat)  { Stream << "Repeat|"; }
    if (Flags & ERawInputStateBits::Release) { Stream << "Release|"; }

    LString Result{Stream.str()};
    if (Result.empty()) { Result = "Identity"; }
    return Result;
}

struct LRawInput final
{
    LPhysicalKey PhysicalKey;
    EModFlags Mods;
    f32 Value{ 1.0f };
    ERawInputStateFlags State;

    /* Do not merge this two with constexpr exprs as this confuses clangds intellisense... */
    template<ERawInputStateFlags State>
    bool Is() const noexcept
    {
        return !!(this->State & State);
    }
    template<ERawInputStateFlags State, typename... TKeys> requires(sizeof...(TKeys) > 0
        && (... && (std::same_as<std::remove_cvref_t<TKeys>, LPhysicalKey>
             || std::same_as<std::remove_cvref_t<TKeys>, std::optional<LPhysicalKey>>)))
    bool Is(TKeys&&... Keys) const noexcept
    {
        return !!(this->State & State) && ((this->PhysicalKey == Keys) || ...);
    }

    LString ToString() const noexcept
    {
        return algo::sprintf("{{{}: {:.2f} M:{} S:{}}}"
            , this->PhysicalKey.ToString(), this->Value, LexToString(this->Mods), LexToString(this->State));
    }
};

} /* ~Namespace Jafg */
