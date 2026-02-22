// Copyright mzoesch. All rights reserved.

#include "Fw/FwSupremePolicies.h"
#include "Framework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DebugCameraComponent.h"

void Jgc::AFwSupremePolicies::OnWorldPreInit()
{
    Super::OnWorldPreInit();

    this->GetWorld().SetBackgroundColor(LinearColors::DeepSkyBlue);

    Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{this->GetWorld()})->EmplaceComponent<Jafg::AStaticMeshComponent>([](Jafg::AStaticMeshComponent& Comp)
    {
        Comp.Create({
            .MeshPath = "Content/Models/viking_room.obj",
            .TexturePath = "Content/Textures/viking_room.png",
            });

        Comp.SetTranslation(LWorldVec3{0,0,-2});
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
