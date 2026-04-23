// Copyright mzoesch. All rights reserved.

#include "Fw/FwSupremePolicies.h"
#include "Framework/Actor.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/PersonaController.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DebugCameraComponent.h"
#include "User/LocalEgo.h"
#include "User/Frontend/JgcPauseScreen.h"
#include "Nodes/WorldNode.h"

void Jgc::AFwSupremePolicies::OnWorldPreInit()
{
    Super::OnWorldPreInit();

    auto& Frontend{this->GetMutableLocalEgo().GetFrontend()};
    auto MaterialInstance{Frontend.GetSubsystemChecked<Jafg::JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.MetallicGold")};

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

void Jgc::AFwSupremePolicies::OnPersonaControllerCreated(Jafg::APersonaController& Pc)
{
    Super::OnPersonaControllerCreated(Pc);

    if (Pc.IsLocallyPossessed())
    {
        if (Pc.IsOwningNodeValid())
        {
            // TODO:
            //      Parent have TJxxUnique<T>.
            //      A widget is constructed as soon as it is emplaced into a viewport.
            //      If a child is emplaced into its parent. Query for outer viewport. If exits -> construct, else defer.
            //      Once attached. Children cannot be removed unless they are destroyed. They can never change their parent again.

            Jafg::ConstructWidget(Jafg::TNodeStaticInit<WPauseScreen>{Pc.GetOwningNodeChecked()->GetViewport()});
        }
        else
        {
            LOG_WARNING(LogJgc, "PersonaController [{}] does not have a valid owning surface", Pc.GetNameAsString());
        }
    }

    return;
}

Jafg::TJxxUnique<Jafg::APawn> Jgc::AFwSupremePolicies::GetPawnForPersonaController(Jafg::APersonaController const& Pc)
{
    auto Pawn{Super::GetPawnForPersonaController(Pc)};
    if (Pawn.get() == nullptr)
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
