// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Core/Name.h"
#include "Async/TaskUtility.h"

namespace Jafg::Private
{

TdhArray<LSimpleString>& GetStaticNameContainer()
{
    static TdhArray<LSimpleString> StaticNameContainer;
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

int32 GetStaticNameCount()
{
    return GetStaticNameContainer().GetSize();
}

const LSimpleString& GetStaticNameByIndex(const int32 InIndex)
{
    return GetStaticNameContainer()[InIndex];
}

LName RegisterStaticName(const LSimpleString& InName)
{
    check( GNameRegistry == nullptr )
    GetStaticNameContainer().Emplace(InName);
    return LNameRegistry::GetNameByValue(GetStaticNameCount());
}

LName RegisterStaticName(LSimpleString&& InName)
{
    check( GNameRegistry == nullptr )
    GetStaticNameContainer().Emplace(std::move(InName));
    return LNameRegistry::GetNameByValue(GetStaticNameCount());
}

} /* ~Namespace Jafg::Private */

namespace Jafg
{

ENGINE_API LName LName::NoName = LName::LName(NO_NAME);

const LSimpleString& LName::ToString() const
{
    check( Private::GNameRegistry )
    return Private::GNameRegistry->GetRealName(*this);
}

} /* ~Namespace Jafg */

Jafg::Private::LNameRegistry::~LNameRegistry()
{
    LOG_VERBOSE(LogNames, "Deleting {} names from the registry.", this->Names.GetSize())
    this->Names.Empty();
    return;
}

Jafg::LName Jafg::Private::LNameRegistry::GetName(const LSimpleString& InName, const bool bConvertToLower /* = true */) const
{
    if (bConvertToLower)
    {
        const LSimpleString LowerName = InName.GetLowerCase();
        int32 Index = 0;
        if (const LSimpleString* Ref = this->Names.FindRef(LowerName, Index); Ref)
        {
            return { static_cast<LUnderlyingName>(Index + 1) };
        }
    }
    else
    {
        int32 Index = 0;
        if (const LSimpleString* Ref = this->Names.FindRef(InName, Index); Ref)
        {
            return { static_cast<LUnderlyingName>(Index + 1) };
        }
    }

    return LName::NoName;
}

bool Jafg::Private::LNameRegistry::IsNameRegistered(const LSimpleString& InName, const bool bConvertToLower /* = true */) const
{
    if (bConvertToLower)
    {
        const LSimpleString LowerName = InName.GetLowerCase();
        return this->Names.Contains(LowerName);
    }

    return this->Names.Contains(InName);
}

bool Jafg::Private::LNameRegistry::RegisterName(const LSimpleString& InName)
{
    check( Tasks::IsOnMasterThread() )

    LSimpleString LowerName = InName.GetLowerCase();

    if (this->IsNameRegistered(LowerName, false))
    {
        LOG_WARNING(LogNames, "Name [{}] is already registered.", InName)
        return false;
    }

    this->Names.Emplace(std::move(LowerName));
    return true;
}

Jafg::LName Jafg::Private::LNameRegistry::RegisterAndGetName(const LSimpleString& InName)
{
    if (this->RegisterName(InName))
    {
        return { static_cast<LUnderlyingName>(this->Names.GetSize()) };
    }

    const LSimpleString LowerName = InName.GetLowerCase();

    if (this->IsNameRegistered(InName, false))
    {
        return this->GetName(InName, false);
    }

    return LName::NoName;
}
