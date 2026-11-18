// Copyright mzoesch. All rights reserved.

#include "Framework/SupremePolicies.h"
#include "Framework/PersonaController.h"
#include "Framework/Actor.h"
#include "Framework/Editor.h"
#include "Framework/AvatarComponent.h"
#include "Framework/AvatarStart.h"
#include "Framework/EyeComponent.h"

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

    auto Result{TJxxUnique<APersonaController>{
        SpawnObject(CastTo<APersonaController>{}, {this->GetWorld(), this->PersonaControllerClass.GetClassOrDefault()})
        }};
#if JAFG_WITH_EDITOR
    Result->EmplaceComponent<ADebugDrawerComponent>();
#endif /* JAFG_WITH_EDITOR */
    return Result;
}

void Jafg::ASupremePolicies::OnPersonaControllerCreated(APersonaController& Pc)
{
    if (!this->bCreateAvatar)
    {
        return;
    }

    LOG_VERBOSE(LogWorld, "Creating avatar for controller [{}].", Pc.GetNameAsString())

    auto Actor{this->SpawnAvatarForPersonaController(Pc)};
    if (!Actor)
    {
        LOG_VERBOSE(LogWorld, "No avatar for controller [{}].", Pc.GetNameAsString())
        return;
    }
    check(IsValidSlow(&this->GetOuter(), Actor.get()))

    auto* Avatar{Actor->GetAvatar()};
    if (!Avatar)
    {
        LOG_FATAL(LogWorld, "[{}]: Spawned actor [{}] for controller [{}] has no [{}]."
            , this->GetNameAsString(), Actor->GetNameAsString(), Pc.GetNameAsString(), algo::type_name<AAvatarComponent>()
            )
    }

    LOG_VERBOSE(LogWorld, "Created avatar [{}] for controller [{}]. Starting possess process."
        , Actor->GetNameAsString(), Pc.GetNameAsString())
    Pc.AdoptActor(std::move(Actor));
    Pc.Possess(Avatar);
}

TJxxUnique<Jafg::AActor> Jafg::ASupremePolicies::SpawnAvatarForPersonaController(APersonaController const& Pc)
{
    TSubclassOf<AActor> Class{this->DefaultAvatarClass};
    bool bEditor{};
#if JAFG_WITH_EDITOR
    if (Pc.GetComponent<AEditorPersonaControllerComponent>())
    {
        check(Pc.IsLocallyPossessed())
        Class = this->EditorDefaultAvatarClass;
        bEditor = true;
    }
#endif /* JAFG_WITH_EDITOR */

    auto Exec{SpawnDeferredObject(CastTo<AActor>{}, {this->GetWorld(), Class.GetClassOrDefault()})};
    /* Components of avatars (e.g., movement or input) usually tick in both. */
    Exec->SetCanEverTick(true);
    Exec->SetCanEverPhysicsTick(true);

    if (!Exec->HasRootComponent())
    {
        Exec->EmplaceRootComponent<ASceneComponent>();
    }
    if (!AEyeComponent::FindFirstEyeIn(Exec->GetRootComponent()))
    {
        Exec->EmplaceSceneComponentTo<AEyeComponent>(Exec->GetRootComponent());
    }
    if (!Exec->GetAvatar())
    {
        Exec->EmplaceComponent<AAvatarComponent>();
    }

    if (!this->bIgnoreAvatarStarts)
    {
        if (AAvatarStart const* Start{this->GetAvatarStartFor(*Exec)})
        {
            if (bEditor)
            {
                Exec->GetRootComponent().SetLocalTransformByTeleport(Start->GetRootComponent().GetLocalTransform()); // TODO: Instead of that we should store the last editor location and load it.
            }
            else
            {
                Exec->GetRootComponent().SetLocalTranslationByTeleport(Start->GetRootComponent().GetLocalTranslation());
            }
        }
    }

    TJxxUnique<AActor> Result{&*Exec};
    Exec.finalize();
#if JAFG_WITH_EDITOR
    if (bEditor)
    {
        Result->EmplaceComponent<AEditorCameraComponent>();
    }
#endif /* JAFG_WITH_EDITOR */
    return Result;
}

Jafg::AAvatarStart const* Jafg::ASupremePolicies::GetAvatarStartFor(AActor const& Avatar)
{
    for (auto const& Employee: this->GetWorld().GetEmployees())
    {
        if (auto* Result{Employee->As<AAvatarStart>()})
        {
            if (Result->CanAccept(Avatar))
            {
                return Result;
            }
        }
    }
    return nullptr;
}
