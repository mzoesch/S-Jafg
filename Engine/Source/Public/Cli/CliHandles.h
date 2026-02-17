// Copyright mzoesch. All rights reserved.

#pragma once

#include "Cli/CliObject.h"

namespace Jafg
{

class LCommandLineInterface;
struct LCliObjectHandle;
typedef LCliObjectHandle LCliTypeHandle;
typedef LCliObjectHandle LCliCommandHandle;
typedef LCliObjectHandle LCliVariableHandle;
struct LCliObjectRaiiHandle;
typedef LCliObjectRaiiHandle LCliTypeRaiiHandle;
typedef LCliObjectRaiiHandle LCliCommandRaiiHandle;
typedef LCliObjectRaiiHandle LCliVariableRaiiHandle;
namespace ECliType { enum Type : u8; }

//#
//# A weak handle to any CLI object.
//#
struct LCliObjectHandle final
{
    friend LCommandLineInterface;
    friend LCliObjectRaiiHandle;

    FORCEINLINE constexpr LCliObjectHandle() noexcept = default;
    FORCEINLINE constexpr  explicit LCliObjectHandle(const LCliObjectUuid InUuid) noexcept : Uuid(InUuid) {}
    FORCEINLINE constexpr LCliObjectHandle(LCliObjectHandle const& Other) : Uuid(Other.Uuid) { }
    FORCEINLINE constexpr LCliObjectHandle(LCliObjectHandle&& Other) noexcept : Uuid(Other.Uuid) { Other.Uuid = LCliObject::NoUuid; }
    FORCEINLINE constexpr LCliObjectHandle& operator=(const LCliObjectHandle& Rhs) noexcept { this->Uuid = Rhs.Uuid; return *this; }
    FORCEINLINE constexpr LCliObjectHandle& operator=(LCliObjectHandle&& Rhs) noexcept { this->Uuid = Rhs.Uuid; Rhs.Uuid = LCliObject::NoUuid; return *this; }
    FORCEINLINE constexpr ~LCliObjectHandle() noexcept = default;

    FORCEINLINE constexpr bool IsValid() const noexcept { return this->Uuid != LCliObject::NoUuid; }
    FORCEINLINE constexpr void Reset() noexcept { this->Uuid = LCliObject::NoUuid; }

    template<typename TField> FORCEINLINE void GetValue(TField* Destination) const;

private:

    ENGINE_API LCommandLineInterface* GetCommandLineInterface() const noexcept;

    LCliObjectUuid Uuid{ LCliObject::NoUuid };
};

//#
//# A raii handle to any CLI object.
//#
struct LCliObjectRaiiHandle final
{
    friend LCommandLineInterface;

    FORCEINLINE constexpr LCliObjectRaiiHandle() noexcept = default;
    FORCEINLINE constexpr explicit LCliObjectRaiiHandle(const LCliObjectUuid InUuid) noexcept : Uuid(InUuid) {}
    FORCEINLINE constexpr LCliObjectRaiiHandle(LCliObjectRaiiHandle&& Other) noexcept : Uuid(Other.Uuid) { Other.Uuid = LCliObject::NoUuid; }
    FORCEINLINE constexpr LCliObjectRaiiHandle& operator=(LCliObjectRaiiHandle&& Rhs) noexcept { this->Uuid = Rhs.Uuid; Rhs.Uuid = LCliObject::NoUuid; return *this; }
    FORCEINLINE constexpr LCliObjectRaiiHandle(LCliObjectHandle&& Handle) noexcept : Uuid(Handle.Uuid) { Handle.Uuid = LCliObject::NoUuid; }
    PROHIBIT_COPY(LCliObjectRaiiHandle)
    FORCEINLINE ~LCliObjectRaiiHandle() noexcept { this->Reset(); }

    FORCEINLINE constexpr explicit operator LCliObjectHandle() const noexcept { return LCliObjectHandle{this->Uuid}; }

    FORCEINLINE constexpr bool IsValid() const noexcept { return this->Uuid != LCliObject::NoUuid; }
    //# Release ownership over the handle.
    FORCEINLINE constexpr void Release() noexcept { this->Uuid = LCliObject::NoUuid; }
    //# Reset the handle while freeing owned resources.
    FORCEINLINE constexpr void Reset() noexcept;

    template<typename TField> FORCEINLINE void GetValue(TField* Destination) const;

private:

    ENGINE_API LCommandLineInterface* GetCommandLineInterface() const noexcept;

    LCliObjectUuid Uuid{ LCliObject::NoUuid };
};

namespace ECliType
{

    enum Type : u8
    {
        Ty,
        Command,
        Variable,
    };

} /* ~Namespace EObjectType */

} /* ~Namespace Jafg */
