// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Core/Name.h"
#include "Async/TaskUtility.h"

namespace Jafg::Private
{

TArray<LString>& GetStaticNameContainer()
{
    static TArray<LString> StaticNameContainer;
    return StaticNameContainer;
}

ENGINE_API LNameRegistry* GNameRegistry = nullptr;

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

ENGINE_API LName LName::NoName = LName(NO_NAME);
ENGINE_API LString LName::NoNameStringRepresentation = "NoName";

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

Jafg::LName Jafg::Private::LNameRegistry::GetName(const LString& InName, const bool bConvertToLower /* = true */) const
{
    if (bConvertToLower)
    {
        const LString LowerName = InName.GetLower();
        if (const TArray<LString>::SizeType Index { this->Names.FindIndex(LowerName) }; Index != this->Names.GetSize())
        {
            return { static_cast<LUnderlyingName>(Index + 1) };
        }
    }
    else
    {
        if (const TArray<LString>::SizeType Index { this->Names.FindIndex(InName) }; Index != this->Names.GetSize())
        {
            return { static_cast<LUnderlyingName>(Index + 1) };
        }
    }

    return LName::NoName;
}

bool Jafg::Private::LNameRegistry::IsNameRegistered(const LString& InName, const bool bConvertToLower /* = true */) const
{
    if (bConvertToLower)
    {
        const LString LowerName = InName.GetLower();
        return this->Names.Contains(LowerName);
    }

    return this->Names.Contains(InName);
}

bool Jafg::Private::LNameRegistry::RegisterName(const LString& InName)
{
    check( Tasks::IsOnMasterThread() )

    LString LowerName = InName.GetLower();

    if (this->IsNameRegistered(LowerName, false))
    {
        LOG_WARNING(LogNames, "Name [{}] is already registered.", InName)
        return false;
    }

    this->Names.Emplace(std::move(LowerName));
    LOG_TRACE(LogNames, "Registered name [{}].", *this->Names.GetLast())
    return true;
}

Jafg::LName Jafg::Private::LNameRegistry::RegisterAndGetName(const LString& InName)
{
    check( Tasks::IsOnMasterThread() )

    const LString LowerName = InName.GetLower();

    if (const LName InRepo = this->GetName(LowerName, false); InRepo.IsSet())
    {
        return InRepo;
    }

    if (this->RegisterName(InName))
    {
        return { static_cast<LUnderlyingName>(this->Names.GetSize()) };
    }

    jassertNoEntry()
    return LName::NoName;
}
