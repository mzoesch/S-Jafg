// Copyright mzoesch. All rights reserved.

#pragma once

#include "Misc/Tag.h"
#include "Async/TaskUtility.h"

namespace Jafg
{

namespace Private
{

struct LNameRegistryTag;
struct LOmniVitaNameRegistry;

struct LNameRegistryTag : public Lal::TTag<u16>
{
    using Super = Lal::TTag<u16>;
    using Super::Super;
    ENGINE_API LString ToString() const noexcept;
};

//# Valid at the start of the static storage initialization phase from the runtime until the very end.
ENGINE_API LOmniVitaNameRegistry& GetNameRegistry() noexcept;

struct LOmniVitaNameRegistry : public Lal::TTagRegistry<LNameRegistryTag>
{
    using Super = TTagRegistry;

    FORCEINLINE TagType RegisterOrGet(Trait::CString auto&& InRepr) noexcept;
    template <LSize N>
    FORCEINLINE TagType RegisterOrGet(const char(&InRepr)[N]) noexcept
    {
        return this->RegisterOrGet(LStringView{InRepr, N - 1});
    }
};

} /* ~Namespace Private */

} /* ~Namespace Jafg */

//#
//# A name maps a string to a unique integer. Names are case-insensitive and are stored in a global registry.
//# Names behave trivially in any context.
//# Core names allocated at module initialization time are not allocated deterministically. The underlying name
//# might differ between runs.
//# Names are safe to use in networked environments.
//#
typedef Jafg::Private::LOmniVitaNameRegistry::TagType LName;

//#
//# Dynamically register a name depending on context at runtime.
//# If the name is already registered, it will return that.
//#
#define MAKE_NAME(Name)             ::Jafg::Private::GetNameRegistry().RegisterOrGet(Name)

//#
//# Get a name by its string representation.
//# If the name is not registered, it will return #LName::NO_TAG.
//#
#define GET_NAME(Name)              ::Jafg::Private::GetNameRegistry().GetTag(Name)
#define GET_NAME_CHECKED(Name)      ::Jafg::Private::GetNameRegistry().GetTagChecked(Name)
#define GET_NAME_ASSERTED(Name)     ::Jafg::Private::GetNameRegistry().GetTagAsserted(Name)

//#
//# Convert a name to its string representation.
//#
#define GET_NAME_REPR(Name)         ::Jafg::Private::GetNameRegistry().GetReprSafe(Name)
#define GET_NAME_REPR_FAST(Name)    ::Jafg::Private::GetNameRegistry().GetReprFast(Name)

template <>
struct std::formatter<::LName> : std::formatter<LString>
{
    FORCEINLINE auto format
    (
        const ::LName& InName,
        ::std::format_context& InContext
    ) const -> ::std::format_context::iterator
    {
        return ::std::formatter<LString>::format(Jafg::Private::GetNameRegistry().GetReprSafe(InName), InContext);
    }
};

FORCEINLINE Jafg::Private::LOmniVitaNameRegistry::TagType
Jafg::Private::LOmniVitaNameRegistry::RegisterOrGet(Trait::CString auto&& InRepr) noexcept
{
    check( Tasks::IsOnMasterThread() )
    return Super::RegisterOrGet(std::forward<decltype(InRepr)>(InRepr));
}
