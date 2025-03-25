// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

struct LName;

namespace Private
{

class LNameRegistry;

ENGINE_API extern LNameRegistry* GNameRegistry;
ENGINE_API LNameRegistry* GetNameRegistryPtr();
ENGINE_API LNameRegistry& GetNameRegistry();

ENGINE_API void ClearStaticNameContainer();
ENGINE_API auto GetStaticNameCount() -> i32;
ENGINE_API auto GetStaticNameByIndex(const i32 InIndex) -> const LSimpleString&;
ENGINE_API auto RegisterStaticName(const LSimpleString& InName) -> LName;
ENGINE_API auto RegisterStaticName(LSimpleString&& InName) -> LName;

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

    FORCEINLINE LName() = default;
    FORCEINLINE LName(const LName& Other) = default;
    FORCEINLINE LName(LName&& Other) noexcept : UnderlyingName(Other.UnderlyingName) { Other.UnderlyingName = NO_NAME; }
    FORCEINLINE LName& operator=(const LName& Other) = default;
    FORCEINLINE LName& operator=(LName&& Other) noexcept { UnderlyingName = Other.UnderlyingName; Other.UnderlyingName = NO_NAME; return *this; }
    FORCEINLINE ~LName() = default;

    FORCEINLINE static bool IsEqual(const LName& A, const LName& B) { return A.UnderlyingName == B.UnderlyingName; }
    FORCEINLINE bool Equals(const LName& Other) const { return UnderlyingName == Other.UnderlyingName; }
    FORCEINLINE bool operator==(const LName& Other) const { return UnderlyingName == Other.UnderlyingName; }
    FORCEINLINE bool operator!=(const LName& Other) const { return UnderlyingName != Other.UnderlyingName; }

    FORCEINLINE bool IsSet() const { return UnderlyingName != NO_NAME; }

    ENGINE_API const LSimpleString& ToString() const;

    ENGINE_API static LName NoName;
    ENGINE_API static LSimpleString NoNameStringRepresentation;

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
//#
#define MAKE_DYNAMIC_NAME(Name)     ::Jafg::Private::GNameRegistry->RegisterAndGetName(Name)

//#
//# Get a name by its string representation.
//#
#define GET_NAME(Name)              ::Jafg::Private::GNameRegistry->GetName(Name)
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
    ENGINE_API  LName GetName(const LSimpleString& InName, const bool bConvertToLower = true) const;
    ENGINE_API  LName GetName(const LString& InName, const bool bConvertToLower = true) const;
    FORCEINLINE LName GetNameChecked(const LSimpleString& InName, const bool bConvertToLower = true) const;
    FORCEINLINE LName GetNameChecked(const LString& InName, const bool bConvertToLower = true) const;
    FORCEINLINE auto  GetRealNameFast(const LName InName) const -> const LSimpleString& { check( InName.IsSet() ) return this->Names[InName.UnderlyingName - 1]; }
    FORCEINLINE auto  GetRealNameSafe(const LName InName) const -> const LSimpleString&;

    ENGINE_API bool IsNameRegistered(const LSimpleString& InName, const bool bConvertToLower = true) const;
    ENGINE_API bool RegisterName(const LSimpleString& InName);
    ENGINE_API auto RegisterAndGetName(const LSimpleString& InName) -> LName;

    FORCEINLINE i32 GetNameCount() const { return this->Names.GetSize(); }

private:

    TdhArray<LSimpleString> Names;
};

FORCEINLINE LName LNameRegistry::GetNameChecked(const LSimpleString& InName, const bool bConvertToLower /* = true */) const
{
    const LName Name = this->GetName(InName, bConvertToLower);
    check( Name.IsSet() )
    return Name;
}

FORCEINLINE LName LNameRegistry::GetNameChecked(const LString& InName, const bool bConvertToLower /* = true */) const
{
    const LName Name = this->GetName(InName, bConvertToLower);
    check( Name.IsSet() )
    return Name;
}

FORCEINLINE const LSimpleString& LNameRegistry::GetRealNameSafe(const LName InName) const
{
    if (InName.IsSet())
    {
        return this->GetRealNameFast(InName);
    }

    return LName::NoNameStringRepresentation;
}

} /* ~Namespace Private */

} /* ~Namespace Jafg */
