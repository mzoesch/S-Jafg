// Copyright mzoesch. All rights reserved.

#include "Framework/SupremePolicies.h"
#include "Framework/PersonaController.h"
#include "Framework/Actor.h"
#include "Framework/Pawn.h"

TJxxUnique<Jafg::APersonaController> Jafg::ASupremePolicies::OnIncomingConnectionRequest(
      EIncomingConnectionRequest IncomingConnection
    , LString* OutRejectionReason /* = nullptr */
    )
{
    return TJxxUnique<APersonaController>{SpawnObject(CastTo<APersonaController>{}, {this->GetWorld(), this->PersonaControllerClass.GetClassOrDefault()})};
}

void Jafg::ASupremePolicies::OnPersonaControllerCreated(APersonaController& Pc)
{
    if (this->bCreatePawn)
    {
        LOG_VERBOSE(LogWorld, "Creating pawn for controller [{}].", Pc.GetNameAsString())

        auto Pawn{this->GetPawnForPersonaController(Pc)};
        check(IsValidSlow(&this->GetOuter(), Pawn.get()))
        LOG_VERBOSE(LogWorld, "Created pawn [{}] for controller [{}]. Starting possess process.",
            Pawn->GetNameAsString(),
            Pc.GetNameAsString()
            )
        Pc.PossessPawn(std::move(Pawn));
    }

    return;
}

TJxxUnique<Jafg::APawn> Jafg::ASupremePolicies::GetPawnForPersonaController(APersonaController const& Pc)
{
    return TJxxUnique<APawn>{SpawnObject(CastTo<APawn>{}, {this->GetWorld(), this->DefaultPawnClass.GetClassOrDefault()})};
}
