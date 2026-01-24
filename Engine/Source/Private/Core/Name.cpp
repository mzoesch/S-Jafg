// Copyright mzoesch. All rights reserved.

#include "Core/Name.h"

namespace Jafg::Detail
{

LString LNameRegistryTag::ToString() const noexcept
{
    return GetNameRegistry().GetReprSafe(*this);
}

LOmniVitaNameRegistry& GetNameRegistry() noexcept
{
    static LOmniVitaNameRegistry GNameRegistry;
    return GNameRegistry;
}

} /* ~Namespace Jafg::Detail */
