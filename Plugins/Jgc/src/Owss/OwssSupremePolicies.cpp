// Copyright mzoesch. All rights reserved.

#include "Jgc/Owss/OwssSupremePolicies.h"
#include "Framework/PawnStart.h"
#include "Framework/Editor.h"
#include "Jgc/Framework/JgcInputComponent.h"
#include "Jgc/Framework/Vehicle.h"
#include "Jgc/Owss/OwssGenerationSubsystem.h"

void Jgc::AOwssSupremePolicies::OnWorldPostInit()
{
    Super::OnWorldPostInit();
}

void Jgc::AOwssSupremePolicies::OnPersonaControllerCreated(Jafg::APersonaController& Pc)
{
    this->GetWorld().GetSubsystemChecked<JOwssGenerationSubsystem>()->GenerateAsteroids(maths::zero_vector<LWorldVec3>);
    Super::OnPersonaControllerCreated(Pc);
}

TJxxUnique<Jafg::APawn> Jgc::AOwssSupremePolicies::GetPawnForPersonaController(Jafg::APersonaController const& Pc)
{
    auto Result{Super::GetPawnForPersonaController(Pc)};

    if (Result && Pc.IsOwningLackey<Jafg::ELackey::Local>() &&
#if JAFG_WITH_EDITOR
        !Pc.GetComponent<Jafg::AEditorPersonaControllerComponent>()
#endif /* JAFG_WITH_EDITOR */
    )
    {
        auto& InputComponent{Result->EmplaceComponent<AJgcInputComponent>()};
        check(!InputComponent.IsVehicleValid())
        InputComponent.PossessVehicle(&this->GetWorld().GetSubsystemChecked<JOwssGenerationSubsystem>()
            ->SpawnShip(Result->GetRootComponent().GetWorldTranslationSlow()));
    }

    return Result;
}
