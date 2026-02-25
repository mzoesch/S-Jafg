// Copyright mzoesch. All rights reserved.

#include "Fw/FwSupremePolicies.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DebugCameraComponent.h"

void Jgc::AFwSupremePolicies::OnWorldPreInit()
{
    Super::OnWorldPreInit();

    this->GetWorld().SetBackgroundColor(LinearColors::DeepSkyBlue);

    auto& Frontend{this->GetLocalEgo().GetFrontend()};
    auto& MaterialSubsystem{*Frontend.GetSubsystemChecked<Jafg::JMaterialSubsystem>()};
    auto MaterialInstance{MaterialSubsystem.GetInstanceFromMaterialName("Jafg.Identity")};

    Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{this->GetWorld()})
    ->EmplaceComponent<Jafg::AStaticMeshComponent>([MaterialInstance](Jafg::AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/XYZModel.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetTranslation(LWorldVec3{0,0,-10});
    });

    Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{this->GetWorld()})
    ->EmplaceComponent<Jafg::AStaticMeshComponent>([MaterialInstance](Jafg::AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Plane.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetTranslation(LWorldVec3{0,0,-4});
    });

    Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{this->GetWorld()})
    ->EmplaceComponent<Jafg::AStaticMeshComponent>([MaterialInstance](Jafg::AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Cube.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetTranslation(LWorldVec3{2,0,-4});
    });

    Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{this->GetWorld()})
    ->EmplaceComponent<Jafg::AStaticMeshComponent>([MaterialInstance](Jafg::AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Sphere.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetTranslation(LWorldVec3{4,0,-4});
    });

    Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{this->GetWorld()})
    ->EmplaceComponent<Jafg::AStaticMeshComponent>([MaterialInstance](Jafg::AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Icosphere.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetTranslation(LWorldVec3{6,0,-4});
    });

    Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{this->GetWorld()})
    ->EmplaceComponent<Jafg::AStaticMeshComponent>([MaterialInstance](Jafg::AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Cylinder.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetTranslation(LWorldVec3{8,0,-4});
    });

    Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{this->GetWorld()})
    ->EmplaceComponent<Jafg::AStaticMeshComponent>([MaterialInstance](Jafg::AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Cone.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetTranslation(LWorldVec3{10,0,-4});
    });

    Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{this->GetWorld()})
    ->EmplaceComponent<Jafg::AStaticMeshComponent>([MaterialInstance](Jafg::AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Torus.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetTranslation(LWorldVec3{12,0,-4});
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

    Pawn->GetComponentChecked<Jafg::ASceneComponent>()->SetTranslation({0.f, 2.0f, 0.f});
    if (Pc.IsLocallyPossessed())
    {
        Pawn->EmplaceComponent<ADebugCameraComponent>();
    }

    return Pawn;
}
