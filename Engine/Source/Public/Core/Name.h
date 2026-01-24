// Copyright mzoesch. All rights reserved.

#pragma once

#include "Misc/Tag.h"
#include "Misc/TagExt.h"

namespace Jafg::Detail
{

struct LNameRegistryTag : public TTag<u16>
{
    using Super = TTag<u16>;
    using Super::Super;
    ENGINE_API LString ToString() const noexcept;
};

typedef LMasterThreadTagRegistry<LNameRegistryTag> LOmniVitaNameRegistry;


//# Valid at the start of the static storage initialization phase from the runtime until the very end.
ENGINE_API LOmniVitaNameRegistry& GetNameRegistry() noexcept;

} /* ~Namespace Jafg::Detail */

//#
//# A name maps a string to a unique integer. Names are case-sensitive and are stored in a global registry.
//# Names behave trivially in any context.
//# Core names allocated at module initialization time are not allocated deterministically. The underlying name
//# might differ between runs.
//# Names are safe to use in networked environments.
//#
typedef Jafg::Detail::LOmniVitaNameRegistry::TagType LName;

//#
//# Dynamically register a name depending on context at runtime.
//# If the name is already registered, it will return that.
//#
#define MAKE_NAME(Name)             ::Jafg::Detail::GetNameRegistry().RegisterOrGet(Name)

//#
//# Get a name by its string representation.
//# If the name is not registered, it will return #LName::NO_TAG.
//#
#define GET_NAME(Name)              ::Jafg::Detail::GetNameRegistry().GetTag(Name)
#define GET_NAME_CHECKED(Name)      ::Jafg::Detail::GetNameRegistry().GetTagChecked(Name)
#define GET_NAME_ASSERTED(Name)     ::Jafg::Detail::GetNameRegistry().GetTagAsserted(Name)

//#
//# Convert a name to its string representation.
//#
#define GET_NAME_REPR(Name)         ::Jafg::Detail::GetNameRegistry().GetReprSafe(Name)
#define GET_NAME_REPR_FAST(Name)    ::Jafg::Detail::GetNameRegistry().GetReprFast(Name)

template<>
struct std::formatter<::LName> : std::formatter<LString>
{
    FORCEINLINE std::format_context::iterator format(::LName InName, std::format_context& InContext) const
    {
        return std::formatter<LString>::format(Jafg::Detail::GetNameRegistry().GetReprSafe(InName), InContext);
    }
};
