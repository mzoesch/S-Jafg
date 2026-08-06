// Copyright mzoesch. All rights reserved.

#include "Framework/SupremePolicies.h"
#include "Framework/PersonaController.h"
#include "Framework/Actor.h"
#include "Framework/Editor.h"
#include "Framework/Pawn.h"
#include "Framework/PawnStart.h"

std::expected<TJxxUnique<Jafg::APersonaController>, LString> Jafg::ASupremePolicies::OnIncomingConnectionRequest(EConnectionRequest Req)
{
#if JAFG_WITH_EDITOR
    if (Req == Editor)
    {
        auto Result{TJxxUnique<APersonaController>{
            SpawnObject(CastTo<APersonaController>{}, {this->GetWorld(), this->EditorPersonaControllerClass.GetClassOrDefault()})
            }};
        Result->EmplaceComponent<AEditorPersonaControllerComponent>();
        return Result;
    }
#endif /* JAFG_WITH_EDITOR */

    return TJxxUnique<APersonaController>{
        SpawnObject(CastTo<APersonaController>{}, {this->GetWorld(), this->PersonaControllerClass.GetClassOrDefault()})
        };
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
}

TJxxUnique<Jafg::APawn> Jafg::ASupremePolicies::GetPawnForPersonaController(APersonaController const& Pc)
{
#if JAFG_WITH_EDITOR
    if (Pc.GetComponent<AEditorPersonaControllerComponent>())
    {
        check(Pc.IsLocallyPossessed())
        auto Exec{SpawnDeferredObject(CastTo<APawn>{}, {this->GetWorld(), this->EditorDefaultPawnClass.GetClassOrDefault()})};
        if (!this->bIgnorePawnStarts)
        {
            if (APawnStart const* Start{this->GetPawnStartForPawn(*Exec)}) // TODO: Instead of that we should store the last editor location and load it.
            {
                Exec->GetRootComponent().SetTransform(Start->GetRootComponent().GetTransform());
            }
        }
        TJxxUnique<APawn> Result{&*Exec};
        Exec.finalize();
        Result->EmplaceComponent<AEditorCameraComponent>();
        return Result;
    }
#endif /* JAFG_WITH_EDITOR */

    auto Result{SpawnDeferredObject(CastTo<APawn>{}, {this->GetWorld(), this->DefaultPawnClass.GetClassOrDefault()})};
    if (!this->bIgnorePawnStarts)
    {
        if (APawnStart const* Start{this->GetPawnStartForPawn(*Result)})
        {
            Result->GetRootComponent().SetTransform(Start->GetRootComponent().GetTransform());
        }
    }
    return TJxxUnique<APawn>{&*Result};
}

Jafg::APawnStart const* Jafg::ASupremePolicies::GetPawnStartForPawn(APawn const& Pawn)
{
    for (auto const& Employee: this->GetWorld().GetEmployees())
    {
        if (auto* Result{Employee->As<APawnStart>()})
        {
            if (Result->CanAcceptPawn(Pawn))
            {
                return Result;
            }
        }
    }
    return nullptr;
}
