// Copyright mzoesch. All rights reserved.

#include "Core/Name.h"
#include "Async/TaskUtility.h"

namespace Jafg::Private
{

TArray<LString>& GetStaticNameContainer()
{
    static TArray<LString> StaticNameContainer;
    return StaticNameContainer;
}

//#
//# Q: Why is this not static initialized?
//#
//# A: To preserve consistency over non-deterministic plugin loading order and static initialization order, as well
//#    as coherence in unity builds.
//#
ENGINE_API LNameRegistry* GNameRegistry { nullptr };

LNameRegistry* GetNameRegistryPtr()
{
    return GNameRegistry;
}

LNameRegistry& GetNameRegistry()
{
    check( GNameRegistry )
    return *GNameRegistry;
}

void ClearStaticNameContainer()
{
    GetStaticNameContainer().Empty();
}

i32 GetStaticNameCount()
{
    return GetStaticNameContainer().GetSize();
}

const LString& GetStaticNameByIndex(const i32 InIndex)
{
    return GetStaticNameContainer()[InIndex];
}

LName RegisterStaticName(const LString& InName)
{
    check( GNameRegistry == nullptr )
    GetStaticNameContainer().Emplace(InName);
    return LNameRegistry::GetNameByValue(GetStaticNameCount());
}

LName RegisterStaticName(LString&& InName)
{
    check( GNameRegistry == nullptr )
    GetStaticNameContainer().Emplace(std::move(InName));
    return LNameRegistry::GetNameByValue(GetStaticNameCount());
}

} /* ~Namespace Jafg::Private */

namespace Jafg
{

ENGINE_API LName LName::NoName { NO_NAME };
ENGINE_API LString LName::NoNameStringRepresentation { "NoName" };

const LString& LName::ToString() const
{
    check( Private::GNameRegistry )
    return Private::GNameRegistry->GetRealNameSafe(*this);
}

} /* ~Namespace Jafg */

Jafg::Private::LNameRegistry::~LNameRegistry()
{
    LOG_VERBOSE(LogNames, "Deleting {} names from the registry.", this->Names.GetSize())
    this->Names.Empty();
    return;
}

Jafg::LName Jafg::Private::LNameRegistry::GetName(const LString& InName) const
{
    if (const TArray<LString>::SizeType Idx { this->Names.FindIndex(InName) }; Idx != this->Names.GetSize())
    {
        return { static_cast<LUnderlyingName>(Idx + 1) };
    }

    return LName::NoName;
}

bool Jafg::Private::LNameRegistry::IsNameRegistered(const LString& InName) const
{
    return this->Names.Contains(InName);
}

Jafg::LName Jafg::Private::LNameRegistry::RegisterAndGetName(const LString& InName)
{
    check( Tasks::IsOnMasterThread() )

    if (const LName Name { this->GetName(InName) }; Name.IsSet())
    {
        return Name;
    }

    if (this->RegisterName(InName))
    {
        return { static_cast<LUnderlyingName>(this->Names.GetSize()) };
    }

    unreachable()
}

void Jafg::Private::LNameRegistry::Destroy()
{
    LOG_VERBOSE(LogNames, "Destroying name registry with [{}] names.", this->Names.GetSize())
    this->Names.Empty();

    return;
}

bool Jafg::Private::LNameRegistry::RegisterName(const LString& InName)
{
    check( Tasks::IsOnMasterThread() )

    if (this->IsNameRegistered(InName))
    {
        LOG_WARNING(LogNames, "Name [{}] is already registered.", InName)
        return false;
    }

    this->Names.Emplace(std::move(InName));
    LOG_TRACE(LogNames, "Registered name [{}].", *this->Names.GetLast())
    return true;
}
