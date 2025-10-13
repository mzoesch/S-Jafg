// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/BuildToolMacros.h"
#include "Engine/CxxClassMacros.h"

namespace Jafg
{

class LClassOuter;

//#
//# Helper struct to for CxxClass construction.
//#
struct LCxxObjectInitializer final
{
    LCxxObjectInitializer() noexcept = delete;
    constexpr explicit LCxxObjectInitializer(LClassOuter& Outer) noexcept
        : Outer(Outer)
    {
    }

    PROHIBIT_REALLOC_OF_ANY_FORM(LCxxObjectInitializer)

    ~LCxxObjectInitializer() noexcept = default;

    LClassOuter& Outer;
};

ENGINE_API LCxxObjectInitializer GetDefaultObjectInitializer() noexcept;

MAKE_DELEGATE_SIGNATURE(LSetCxxClassField, void, LString const& Value)
MAKE_DELEGATE_SIGNATURE(LGetCxxClassField, LString)

struct LCxxClassField final
{
    LStringView Identifier;
    LSetCxxClassField Set;
    LGetCxxClassField Get;
};

} /* ~Namespace Jafg */
