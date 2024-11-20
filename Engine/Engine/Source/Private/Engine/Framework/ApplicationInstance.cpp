// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Engine/Engine.h"
#include "Engine/ObjectContext.h"
#include "Subsystems/SubsystemCollection.h"
#include "Engine/Framework/ApplicationInstance.h"
#include "Subsystems/ApplicationInstanceSubsystem.h"

void Jafg::LApplicationInstance::Initialize()
{
    checkSlow( this->Context == nullptr )

    this->Context = new ::Jafg::Private::LObjectContext();
    this->Context->SetHumanReadableName("ApplicationInstance");

    checkSlow( this->Collection == nullptr )
    this->Collection = new LSubsystemCollection(this->Context);
    this->Collection->LocateAllSubsystemsOfClass(JApplicationInstanceSubsystem::StaticClass());
    this->Collection->InitializeSubsystems();

    return;
}

void Jafg::LApplicationInstance::TearDown()
{
    if (ensure(this->Collection))
    {
        this->Collection->TearDownSubsystems();
        delete this->Collection;
        this->Collection = nullptr;
    }

    check( this->Context )
    this->Context->TearDownContext();
    delete this->Context;
    this->Context = nullptr;

    return;
}

Jafg::LEngine* Jafg::LApplicationInstance::GetEngine()
{
    checkSlow( GEngine )
    return GEngine;
}

const Jafg::LEngine* Jafg::LApplicationInstance::GetEngine() const
{
    checkSlow( GEngine )
    return GEngine;
}

Jafg::JApplicationInstanceSubsystem* Jafg::LApplicationInstance::GetSubsystem(const LObjectClass* InStaticClass)
{
    return this->Collection->GetSubsystem<JApplicationInstanceSubsystem>(InStaticClass);
}

const Jafg::JApplicationInstanceSubsystem* Jafg::LApplicationInstance::GetSubsystem(const LObjectClass* InStaticClass) const
{
    return this->Collection->GetSubsystem<JApplicationInstanceSubsystem>(InStaticClass);
}

Jafg::JApplicationInstanceSubsystem* Jafg::LApplicationInstance::GetCheckedSubsystem(const LObjectClass* InStaticClass)
{
    return this->Collection->GetCheckedSubsystem<JApplicationInstanceSubsystem>(InStaticClass);
}

const Jafg::JApplicationInstanceSubsystem* Jafg::LApplicationInstance::GetCheckedSubsystem(const LObjectClass* InStaticClass) const
{
    return this->Collection->GetCheckedSubsystem<JApplicationInstanceSubsystem>(InStaticClass);
}
