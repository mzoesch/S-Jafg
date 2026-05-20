// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

struct LCliObjectHandle;
class LCommandLineInterface;

typedef u32 LCliObjectUuid;

//#
//# Abstract base class for all command line interface objects.
//#
class LCliObject
{
    friend LCommandLineInterface;

public:

    inline static constexpr LCliObjectUuid NoUuid{};

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
        check( InOther.Uuid == NoUuid && InOther.Identifier.empty() && InOther.Help.empty() )
        return;
    }
    FORCEINLINE LCliObject& operator=(LCliObject&& InOther) noexcept
    {
        this->Uuid = InOther.Uuid;
        InOther.Uuid = NoUuid;
        this->Identifier = std::move(InOther.Identifier);
        this->Help = std::move(InOther.Help);
        check( InOther.Uuid == NoUuid && InOther.Identifier.empty() && InOther.Help.empty() )
        return *this;
    }
    virtual ~LCliObject() = default;

    FORCEINLINE auto GetIdentifier() const -> LString const& { return this->Identifier; }
    FORCEINLINE void SetIdentifier(const LString& InIdentifier) { this->Identifier = InIdentifier; }
    FORCEINLINE auto GetHelp() const -> LString { return this->Help; }
    FORCEINLINE void SetHelp(LString const& InHelp) { this->Help = InHelp; }

    FORCEINLINE bool IsUuidValid() const { return this->Uuid != NoUuid; }

    FORCEINLINE std::strong_ordering operator<=>(LCliObject const& InOther) const
    {
        return this->Identifier <=> InOther.Identifier;
    }

protected:

    ENGINE_API LCliObject* GetRegisteredObjectByUuid() const;

    ENGINE_API  void ExpandToUuid();
    FORCEINLINE constexpr auto GetUuid() const -> LCliObjectUuid { return this->Uuid; }

private:

    LCliObjectUuid Uuid{ NoUuid };
    LString Identifier;
    LString Help;
};

} /* ~Namespace Jafg */
