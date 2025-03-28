// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

class LCommandLineInterface;

typedef u32 LCliObjectUuid;

//#
//# Abstract base class for all command line interface objects.
//#
class LCliObject
{
    friend LCommandLineInterface;

public:

    static constexpr LCliObjectUuid NoUuid { 0 };

    FORCEINLINE LCliObject() = default;
    FORCEINLINE LCliObject(const LString& InName) : Identifier(InName) { }
    FORCEINLINE LCliObject(const LString& InName, const LString& InHelp) : Identifier(InName), Help(InHelp) { }
    PROHIBIT_COPY(LCliObject)
    FORCEINLINE LCliObject(LCliObject&& InOther) noexcept
    {
        this->Uuid = InOther.Uuid;
        InOther.Uuid = NoUuid;
        this->Identifier = std::move(InOther.Identifier);
        this->Help = std::move(InOther.Help);
        check( InOther.Uuid == NoUuid && InOther.Identifier.IsEmpty() && InOther.Help.IsEmpty() )
        return;
    }
    FORCEINLINE LCliObject& operator=(LCliObject&& InOther) noexcept
    {
        this->Uuid = InOther.Uuid;
        InOther.Uuid = NoUuid;
        this->Identifier = std::move(InOther.Identifier);
        this->Help = std::move(InOther.Help);
        check( InOther.Uuid == NoUuid && InOther.Identifier.IsEmpty() && InOther.Help.IsEmpty() )
        return *this;
    }
    virtual ~LCliObject() = default;

    FORCEINLINE auto GetIdentifier() const -> LString { return this->Identifier; }
    FORCEINLINE void SetIdentifier(const LString& InIdentifier) { this->Identifier = InIdentifier; }
    FORCEINLINE auto GetHelp() const -> LString { return this->Help; }
    FORCEINLINE void SetHelp(const LString& InHelp) { this->Help = InHelp; }

    FORCEINLINE bool IsUuidValid() const { return this->Uuid != NoUuid; }

protected:

    ENGINE_API  void ExpandToUuid();
    FORCEINLINE auto GetUuid() const -> LCliObjectUuid { return this->Uuid; }

private:

    LCliObjectUuid Uuid = NoUuid;
    LString Identifier;
    LString Help;
};

} /* ~Namespace Jafg */
