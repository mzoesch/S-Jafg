// Copyright mzoesch. All rights reserved.

#include "Jgc/Owss/OwssGenerationSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/TextureSubsystem.h"
#include "Framework/Frontend.h"
#include "Framework/StaticMeshComponent.h"
#include "Framework/Actor.h"
#include "Framework/RigidComponent.h"
#include "Jgc/Framework/Vehicle.h"
#include "Rhi/TextureCube.h"

void Jgc::JOwssGenerationSubsystem::Initialize(Jafg::LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    auto& World{this->GetWorld()};

    if (!World.GetSkybox())
    {
        World.MakeSkybox(this->GetMutableEngine().GetLocalEgo().GetFrontend().GetSubsystemChecked<Jafg::JTextureSubsystem>()
            ->CubeFromFile(JgcInfo/"Textures/Jgc/Skies/hiptyc_2020_16k_gal"));
    }

    this->MaterialInstance = World.GetMutableFrontend().GetSubsystemChecked<Jafg::JMaterialSubsystem>()
        ->GetInstanceFromMaterialName("Jafg.MetallicGold");
}

void Jgc::JOwssGenerationSubsystem::GenerateAsteroids(LWorldVec3 Where)
{
    if (this->bGeneratedAsteroids)
    {
        return;
    }
    this->bGeneratedAsteroids = true;

    STAT_FUNCTION()

    check(!!this->MaterialInstance)

    auto PlaceAsteroid{[&](std::size_t Idx, LWorldTrans const& Trans)
    {
        auto& Asteroid{*Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{this->GetWorld()})};
        Asteroid.EditorName = algo::sprintf("Asteroid_{}", Idx);
        Asteroid.EmplaceRootComponent<Jafg::AStaticMeshComponent>([&](Jafg::AStaticMeshComponent& Comp)
        {
            Comp.SetMesh(JgcInfo/"Models/Asteroids/Object_16.glb");
            Comp.SetMaterialInstance(this->MaterialInstance);
        });
        Asteroid.GetRootComponent().SetLocalTransformByTeleport(Trans);
    }};

    auto Engine{std::default_random_engine{}};
    auto TransDistribution{std::uniform_real_distribution<LWorldReal>{-1'000.0f, 1'000.0f}};
    auto RotDistribution{std::uniform_real_distribution<LWorldReal>{0.0f, 360.0f}};
    auto ScaleDistribution{std::uniform_real_distribution<LWorldReal>{0.5f, 20.0f}};

    for (std::size_t Idx{0}; Idx < 5; ++Idx)
    {
        PlaceAsteroid(Idx, LWorldTrans{
            LWorldVec3{TransDistribution(Engine), TransDistribution(Engine), TransDistribution(Engine)} + Where,
            LWorldQuat{LWorldVec3{glm::radians(RotDistribution(Engine)), glm::radians(RotDistribution(Engine)), glm::radians(RotDistribution(Engine))}},
            LWorldVec3{ScaleDistribution(Engine)}
            });
    }
}

Jgc::AVehicleComponent& Jgc::JOwssGenerationSubsystem::SpawnShip(LWorldVec3 Where)
{
    STAT_FUNCTION()
    check(!!this->MaterialInstance)
    return Jafg::SpawnDeferredObject(Jafg::TWorldStaticInit<Jafg::AActor>{this->GetWorld()})([&](Jafg::AActor& Actor)
    {
        Actor.EditorName = "Ship";
        Actor.SetCanEverTick(true);
        Actor.SetCanEverPhysicsTick(true);

        Actor.EmplaceRootComponent<Jafg::ARigidComponent>()
            .SetShouldRender(true);
        Actor.EmplaceSceneComponentTo<Jafg::AStaticMeshComponent>(Actor.GetRootComponent(), [&](Jafg::AStaticMeshComponent& Comp)
        {
            Comp.SetMesh(JgcInfo/"Models/SF-28a.glb");
            Comp.SetMaterialInstance(this->MaterialInstance);
            Comp.SetLocalTranslationByTeleport(Where);
        });

        Actor.EmplaceComponent<AVehicleComponent>([](AVehicleComponent& Comp)
        {
            Comp.SetVehicleType(EVehicle::Dynamic3D);
        });
    })->GetComponentChecked<AVehicleComponent>();
}
