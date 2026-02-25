// Copyright mzoesch. All rights reserved.

#include "Fw/FwSupremePolicies.h"
#include "Framework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DebugCameraComponent.h"

void Jgc::AFwSupremePolicies::OnWorldPreInit()
{
    Super::OnWorldPreInit();

    this->GetWorld().SetBackgroundColor(LinearColors::DeepSkyBlue);

    Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{this->GetWorld()})
    ->EmplaceComponent<Jafg::AStaticMeshComponent>([](Jafg::AStaticMeshComponent& Comp)
    {
        Comp.Create({
            .MeshPath = "Content/Models/NewNew.glb",
            .Material = "viking_room",
            .TextureView = "viking_room",
            });

        Comp.SetTranslation(LWorldVec3{-0.5,-.5,-1});
        Comp.SetRotator(maths::rotator_deg(0,90,0));
    });

    Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{this->GetWorld()})
    ->EmplaceComponent<Jafg::AStaticMeshComponent>([](Jafg::AStaticMeshComponent& Comp)
    {
        Comp.Create({
            .MeshPath = "Content/Models/MyNewNew.glb",
            .Material = "viking_room",
            .TextureView = "viking_room",
            });

        Comp.SetTranslation(LWorldVec3{0,0.6,-1.2});
    });

    Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{this->GetWorld()})
    ->EmplaceComponent<Jafg::AStaticMeshComponent>([](Jafg::AStaticMeshComponent& Comp)
    {
        Comp.Create({
            .MeshPath = "Content/Models/DefaultCube.glb",
            .Material = "viking_room",
            .TextureView = "viking_room",
            });

        Comp.SetTranslation(LWorldVec3{1,0,-1});
        Comp.SetScale(LWorldVec3{0.3f});
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
