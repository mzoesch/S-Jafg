// Copyright mzoesch. All rights reserved.

#include "Fw/FwSupremePolicies.h"
#include "Framework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DebugCameraComponent.h"

void Jgc::AFwSupremePolicies::OnWorldPreInit()
{
    Super::OnWorldPreInit();

    this->GetWorld().SetBackgroundColor(LinearColors::DeepSkyBlue);

    this->RotatingActor = Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{this->GetWorld()});
    this->RotatingActor->EmplaceComponent<Jafg::AStaticMeshComponent>([](Jafg::AStaticMeshComponent& Comp)
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

Jafg::APawn* Jgc::AFwSupremePolicies::GetPawnForPersonaController(Jafg::APersonaController const& Pc)
{
    auto* Pawn{Super::GetPawnForPersonaController(Pc)};
    if (Pawn == nullptr)
    {
        return nullptr;
    }

    if (Pc.IsLocallyPossessed())
    {
        Pawn->EmplaceComponent<ADebugCameraComponent>();
    }

    return Pawn;
}
