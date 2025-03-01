// Copyright mzoesch. All rights reserved.

#include "Engine/Engine.h"
#include "Engine/ObjectContext.h"
#include "Subsystems/SubsystemCollection.h"
#include "Engine/Framework/ApplicationInstance.h"
#include "Subsystems/ApplicationInstanceSubsystem.h"

void Jafg::LApplicationInstance::Initialize()
{
    check( this->Context.GetHumanReadableName().IsEmpty() )
    this->Context.SetHumanReadableName("ApplicationInstance");

    this->Collection.DeferredInitialize(&this->Context);
    this->Collection.InitializeSubsystems(JApplicationInstanceSubsystem::StaticClass());

    return;
}

void Jafg::LApplicationInstance::TearDown()
{
    this->Collection.TearDownSubsystems();

    check( this->Context.GetHumanReadableName().IsEmpty() == false )
    this->Context.TearDownContext();

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
