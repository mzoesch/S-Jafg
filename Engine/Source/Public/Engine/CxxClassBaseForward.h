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
    constexpr explicit LCxxObjectInitializer(LClassOuter& InOuter, LCxxClass& InClass) noexcept
        : Outer(InOuter), Class(InClass)
    {
    }

    PROHIBIT_REALLOC_OF_ANY_FORM(LCxxObjectInitializer)

    ~LCxxObjectInitializer() noexcept = default;

    LClassOuter& Outer;
    LCxxClass& Class;
};

ENGINE_API LCxxObjectInitializer GetDefaultObjectInitializer(LCxxClass& Class) noexcept;

typedef TFunction<void(LString const& Value)> LSetCxxClassField;
typedef TFunction<LString()> LGetCxxClassField;

struct LCxxClassField final
{
    LStringView Identifier;
    LSetCxxClassField Set;
    LGetCxxClassField Get;
};

} /* ~Namespace Jafg */
