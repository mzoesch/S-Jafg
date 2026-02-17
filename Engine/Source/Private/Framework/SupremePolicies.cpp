// Copyright mzoesch. All rights reserved.

#include "Framework/SupremePolicies.h"
#include "Framework/Actor.h"
#include "Framework/PersonaController.h"
#include "Framework/Pawn.h"

Jafg::APersonaController* Jafg::ASupremePolicies::OnIncomingConnectionRequest(
      EIncomingConnectionRequest IncomingConnection
    , LString* OutRejectionReason /* = nullptr */
    )
{
    return SpawnObject(CastTo<APersonaController>{}, {this->GetWorld(), this->PersonaControllerClass.GetClassOrDefault()});
}

void Jafg::ASupremePolicies::OnPersonaControllerCreated(APersonaController* Pc)
{
    check(Pc)

    if (this->bCreatePawn)
    {
        LOG_VERBOSE(LogWorld, "Creating pawn for controller [{}].", Pc->GetNameAsString())

        auto* Pawn{this->GetPawnForPersonaController(*Pc)};
        check(Pawn)
        LOG_VERBOSE(LogWorld, "Created pawn [{}] for controller [{}]. Starting possess process.",
            Pawn->GetNameAsString(),
            Pc->GetNameAsString()
            )
        Pc->PossessPawn(Pawn);
    }

    return;
}

Jafg::APawn* Jafg::ASupremePolicies::GetPawnForPersonaController(APersonaController const& Pc)
{
    return SpawnObject(CastTo<APawn>{}, {this->GetWorld(), this->DefaultPawnClass.GetClassOrDefault()});
}
