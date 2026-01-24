// Copyright mzoesch. All rights reserved.

#include "Framework/SupremePolicies.h"
#include "Framework/Actor.h"
#include "Framework/PersonaController.h"
#include "Framework/Pawn.h"

Jafg::APersonaController* Jafg::JSupremePolicies::OnIncomingConnectionRequest(
      EIncomingConnectionRequest::Type IncomingConnection
    , LString* OutRejectionReason /* = nullptr */
    )
{
    return SpawnDeferredActor<APersonaController>(this->GetWorldChecked(), this->PersonaControllerClass.GetCLassOrDefault());
}

void Jafg::JSupremePolicies::OnPersonaControllerCreated(APersonaController& Pc)
{
    if (this->bCreatePawn)
    {
        LOG_VERBOSE(LogWorld, "Creating pawn for controller [{}].", Pc.GetNameAsString())
        auto* Pawn{ this->SpawnDeferredPawnForPersonaController(Pc) };
        check( Pawn )
        Pc.PossessPawn(Pawn);
        MakeDeferredActorFinal(Pawn);
        LOG_VERBOSE(LogWorld, "Created pawn [{}] for controller [{}].",
            Pawn->GetNameAsString(),
            Pc.GetNameAsString()
            )
    }

    return;
}

Jafg::APawn* Jafg::JSupremePolicies::SpawnDeferredPawnForPersonaController(APersonaController& Pc)
{
    return SpawnDeferredActor<APawn>(this->GetWorldChecked(), this->DefaultPawnClass.GetCLassOrDefault());
}
