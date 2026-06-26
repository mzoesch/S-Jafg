// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

enum struct EInputActionTriggerBits
{
    Identity = 0x0 << 0,

    //#
    //# Marks an action that was just triggered.
    //#
    Triggered = 0x1 << 0,

    //#
    //# Marks an action that is ongoing.
    //# Will be called the same tick where the action was triggered.
    //#
    Ongoing   = 0x1 << 1,

    //#
    //# Marks an action that was just completed.
    //#
    Completed = 0x1 << 2,
};
ENUM_STRUCT_FLAGS(EInputActionTriggerBits, EInputActionTriggerFlags)
inline LStringView LexToString(EInputActionTriggerBits Bit) noexcept
{
    switch (Bit)
    {
    case EInputActionTriggerBits::Identity: return "None";
    case EInputActionTriggerBits::Triggered: return "Triggered";
    case EInputActionTriggerBits::Ongoing: return "Ongoing";
    case EInputActionTriggerBits::Completed: return "Completed";
    }
    std::unreachable();
}
inline LString LexToString(EInputActionTriggerFlags Flags) noexcept
{
    std::stringstream Stream;
    if (Flags & EInputActionTriggerBits::Triggered) { Stream << "Triggered|"; }
    if (Flags & EInputActionTriggerBits::Ongoing) { Stream << "Ongoing|"; }
    if (Flags & EInputActionTriggerBits::Completed) { Stream << "Completed|"; }

    LString Result{Stream.str()};
    if (Result.empty()) { Result = "Identity"; }
    return Result;
}

} /* ~Namespace Jafg */
