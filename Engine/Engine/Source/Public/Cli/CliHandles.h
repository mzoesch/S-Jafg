// Copyright mzoesch. All rights reserved.

#pragma once

#include "Cli/CliObject.h"
#include "Engine/ObjectBaseUtility.h"

namespace Jafg
{

class LCommandLineInterface;
struct LCliObjectHandle;
typedef LCliObjectHandle LCliTypeHandle;
typedef LCliObjectHandle LCliCommandHandle;
typedef LCliObjectHandle LCliVariableHandle;
namespace ECliType { enum Type : u8; }

//#
//# A weak handle to a CLI object.
//#
struct LCliObjectHandle final
{
    friend LCommandLineInterface;

    FORCEINLINE LCliObjectHandle() = default;
    FORCEINLINE LCliObjectHandle(const LCliObjectUuid InUuid) : Uuid(InUuid) { }
    FORCEINLINE LCliObjectHandle(const LCliObjectHandle& InHandle) : Uuid(InHandle.Uuid) { }
    FORCEINLINE LCliObjectHandle(LCliObjectHandle&& InHandle) noexcept : Uuid(InHandle.Uuid) { InHandle.Uuid = LCliObject::NoUuid; }
    FORCEINLINE LCliObjectHandle& operator=(const LCliObjectHandle& InHandle) { this->Uuid = InHandle.Uuid; return *this; }
    FORCEINLINE LCliObjectHandle& operator=(LCliObjectHandle&& InHandle) noexcept { this->Uuid = InHandle.Uuid; InHandle.Uuid = LCliObject::NoUuid; return *this; }
    FORCEINLINE ~LCliObjectHandle() = default;

    FORCEINLINE bool IsValid() const { return this->Uuid != LCliObject::NoUuid; }
    FORCEINLINE void Reset() { this->Uuid = LCliObject::NoUuid; }

    template <typename TField> FORCEINLINE void GetValue(TField* Destination) const;

private:

    ENGINE_API LCommandLineInterface* GetCommandLineInterface() const;

    LCliObjectUuid Uuid { LCliObject::NoUuid };
};

template <>
FORCEINLINE void OnDefaultOnlyMallocMember<LCliObjectHandle>(LCliCommandHandle* MemberField)
{
    MemberField->Reset();
}

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
