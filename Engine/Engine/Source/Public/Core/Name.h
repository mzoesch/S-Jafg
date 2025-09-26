// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"


namespace Jafg
{

struct LName;

} /* ~Namespace Jafg */

using LName = Jafg::LName;

namespace Jafg
{

namespace Private
{

class LNameRegistry;

ENGINE_API extern LNameRegistry* GNameRegistry;
ENGINE_API LNameRegistry* GetNameRegistryPtr();
ENGINE_API LNameRegistry& GetNameRegistry();

ENGINE_API void ClearStaticNameContainer();
ENGINE_API auto GetStaticNameCount() -> i32;
ENGINE_API auto GetStaticNameByIndex(const i32 InIndex) -> const LString&;
ENGINE_API auto RegisterStaticName(const LString& InName) -> LName;
ENGINE_API auto RegisterStaticName(LString&& InName) -> LName;

} /* ~Namespace Private */

typedef u32 LUnderlyingName;

enum : u8 { NO_NAME = 0 };

//#
//# A name maps a string to a unique integer. Names are case-insensitive and are stored in a global registry.
//# Names behave trivially in any context.
//# Core names allocated at module initialization time are not allocated deterministically. The underlying name
//# might differ between runs.
//# Names are safe to use in networked environments.
//#
struct LName
{
    friend Private::LNameRegistry;
    friend std::formatter<LName>;

    FORCEINLINE LName() : UnderlyingName(NO_NAME) { }
    FORCEINLINE LName(const LName& Other) = default;
    FORCEINLINE LName(LName&& Other) noexcept : UnderlyingName(Other.UnderlyingName) { Other.UnderlyingName = NO_NAME; }
    FORCEINLINE LName& operator=(const LName& Other) = default;
    FORCEINLINE LName& operator=(LName&& Other) noexcept { this->UnderlyingName = Other.UnderlyingName; Other.UnderlyingName = NO_NAME; return *this; }
    FORCEINLINE ~LName() = default;

    FORCEINLINE static bool IsEqual(const LName& A, const LName& B) { return A.UnderlyingName == B.UnderlyingName; }
    FORCEINLINE bool Equals(const LName& Other) const { return this->UnderlyingName == Other.UnderlyingName; }
    FORCEINLINE bool operator==(const LName& Other) const { return this->UnderlyingName == Other.UnderlyingName; }
    FORCEINLINE bool operator!=(const LName& Other) const { return this->UnderlyingName != Other.UnderlyingName; }

    FORCEINLINE bool IsSet() const { return this->UnderlyingName != NO_NAME; }

    ENGINE_API const LString& ToString() const;

    ENGINE_API static LName NoName;
    ENGINE_API static LString NoNameStringRepresentation;

    ///////////////////////////////////////////////////////////////////////////////
    // Hashing only. Do not use.
    // This is meaningless as names are not deterministic.
    FORCEINLINE std::strong_ordering operator<=>(const LName& Other) const { return this->UnderlyingName <=> Other.UnderlyingName; }
    // ~Hashing only. Do not use.
    ///////////////////////////////////////////////////////////////////////////////

private:

    FORCEINLINE LName(const LUnderlyingName InUnderlyingName) : UnderlyingName(InUnderlyingName) { }

    LUnderlyingName UnderlyingName;
};

//#
//# Register a name known at compile time.
//#
#define MAKE_STATIC_NAME(Name)      ::Jafg::Private::RegisterStaticName(Name)

//#
//# Dynamically register a name depending on context at runtime.
//# If the name is already registered, it will return that. This is basically a weak form of #GET_NAME.
//#
#define MAKE_DYNAMIC_NAME(Name)     ::Jafg::Private::GNameRegistry->RegisterAndGetName(Name)

//#
//# Get a name by its string representation.
//# If the name is not registered, it will return #LName::NoName.
//#
#define GET_NAME(Name)              ::Jafg::Private::GNameRegistry->GetName(Name)
//#
//# Get a name by its string representation.
//# If #LAL_DO_CHECKS is true, the program will panic, otherwise #LName::NoName will be returned.
//#
#define GET_NAME_CHECKED(Name)      ::Jafg::Private::GNameRegistry->GetNameChecked(Name)

namespace Private
{

class LNameRegistry
{
public:

    LNameRegistry() = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LNameRegistry)
    ENGINE_API ~LNameRegistry();

    static      LName GetNameByValue(LUnderlyingName InUnderlyingName) { return { InUnderlyingName }; }
    ENGINE_API  LName GetName(const LString& InName) const;
    FORCEINLINE LName GetNameChecked(const LString& InName) const;
    FORCEINLINE auto  GetRealNameFast(const LName InName) const -> const LString& { check( InName.IsSet() ) return this->Names[InName.UnderlyingName - 1]; }
    FORCEINLINE auto  GetRealNameSafe(const LName InName) const -> const LString&;

    ENGINE_API bool  IsNameRegistered(const LString& InName) const;
    ENGINE_API LName RegisterAndGetName(const LString& InName);

    FORCEINLINE i32 GetNameCount() const { return this->Names.GetSize(); }

    ENGINE_API void Destroy();

private:

    ENGINE_API bool RegisterName(const LString& InName);

    TArray<LString> Names;
};

FORCEINLINE LName LNameRegistry::GetNameChecked(const LString& InName) const
{
    const LName Name { this->GetName(InName) };
    check( Name.IsSet() )
    return Name;
}

FORCEINLINE const LString& LNameRegistry::GetRealNameSafe(const LName InName) const
{
    if (InName.IsSet())
    {
        return this->GetRealNameFast(InName);
    }

    return LName::NoNameStringRepresentation;
}

} /* ~Namespace Private */

} /* ~Namespace Jafg */

template <>
struct std::formatter<::Jafg::LName> : std::formatter<LString>
{
    FORCEINLINE auto format
    (
        const ::Jafg::LName& InName,
        ::std::format_context& InContext
    ) const -> ::std::format_context::iterator
    {
        return ::std::formatter<LString>::format(InName.ToString(), InContext);
    }
};
