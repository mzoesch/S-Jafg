// Copyright mzoesch. All rights reserved.

#include "Core/Name.h"
#include "Async/TaskUtility.h"

namespace Jafg::Private
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

} /* ~Namespace Jafg::Private */
