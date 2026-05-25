// Copyright mzoesch. All rights reserved.

#include "Framework/Lackey.h"
#include "Engine/Engine.h"

TJxxUnique<Jafg::APersonaController> Jafg::Detail::LLackeyBase::PossessPersonaController(TJxxUnique<APersonaController> New, bool bReleaseOld /* = false*/)
{
    check(GMutableEngine)

    TJxxUnique<APersonaController> Result;

    if (this->IsOwnedPersonaControllerValid())
    {
        this->Controller->Lackey = std::monostate{};
        if (bReleaseOld)
        {
            Result = std::move(this->Controller);
            check(!this->Controller.get())
        }
    }

    this->Controller = std::move(New);

    return Result;
}

TJxxUnique<Jafg::APersonaController> Jafg::LLocalLackey::PossessPersonaController(TJxxUnique<APersonaController> New, bool bReleaseOld /* = false*/)
{
    auto Result{Super::PossessPersonaController(std::move(New), bReleaseOld)};

    if (this->IsOwnedPersonaControllerValid())
    {
        check(std::holds_alternative<std::monostate>(this->Controller->Lackey))
        this->Controller->Lackey = this;
    }

    Detail::GMutableEngine->GetLocalEgo().ForEachMutableSubsystem([New=this->Controller.get()](JLocalEgoSubsystem* Subsystem)
    {
        Subsystem->OnNewPersonaController(New);
    });

    return Result;
}
