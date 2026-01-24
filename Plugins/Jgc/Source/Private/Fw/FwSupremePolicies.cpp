// Copyright mzoesch. All rights reserved.

#include "Fw/FwSupremePolicies.h"
#include "Framework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DebugCameraComponent.h"

void Jgc::JFwSupremePolicies::OnWorldPreInit()
{
    Super::OnWorldPreInit();

    this->GetWorldChecked()->SetBackgroundColor(LinearColors::DeepSkyBlue);

    this->RotatingActor = Jafg::SpawnDeferredActor<Jafg::AActor>(this->GetWorld());
    this->RotatingActor->EmplaceDeferredComponent<Jafg::JStaticMeshComponent>([](Jafg::JStaticMeshComponent& Comp)
    {
        Comp.Create({
            .MeshPath = "Content/Models/viking_room.obj",
            .TexturePath = "Content/Textures/viking_room.png",
            });

        Comp.SetTranslation(LVec3F{2,-5,-4});
        Comp.SetRotator(LWorldQuat{LWorldVec3{
            0, // Pitch
            0, // Yaw
            0 //Roll
            }});
    });

    return;
}

Jafg::APawn* Jgc::JFwSupremePolicies::SpawnDeferredPawnForPersonaController(Jafg::APersonaController& Pc)
{
    auto* Pawn{Super::SpawnDeferredPawnForPersonaController(Pc)};
    if (Pawn == nullptr)
    {
        return nullptr;
    }

    if (Pc.IsLocallyPossessed())
    {
        Pawn->EmplaceDeferredComponent<JDebugCameraComponent>();
    }

    return Pawn;
}
