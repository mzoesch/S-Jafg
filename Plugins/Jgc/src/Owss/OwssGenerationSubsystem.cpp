// Copyright mzoesch. All rights reserved.

#include "Jgc/Owss/OwssGenerationSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/TextureSubsystem.h"
#include "Framework/Frontend.h"
#include "Framework/StaticMeshComponent.h"
#include "Framework/MeshSubsystem.h"
#include "Framework/MeshSceneProjection.h"
#include "Framework/Actor.h"
#include "Framework/LightComponent.h"
#include "Framework/RigidComponent.h"
#include "Framework/AvatarComponent.h"
#include "Framework/EyeComponent.h"
#include "Framework/GimbalComponent.h"
#include "Jgc/Framework/Vehicle.h"
#include "Jgc/Framework/JgcVehicleInputComponent.h"
#include "Jgc/Owss/OwssSupremePolicies.h"
#include "Rhi/TextureCube.h"

void Jgc::JOwssGenerationSubsystem::Initialize(Jafg::LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    auto& World{this->GetWorld()};

    if (!World.GetSkybox())
    {
        World.MakeSkybox(this->GetMutableEngine().GetLocalEgo().GetFrontend().GetSubsystemChecked<Jafg::JTextureSubsystem>()
            ->CubeFromFile(JgcInfo, "Textures/Skies/hiptyc_2020_16k_gal"));
    }

    this->MaterialInstance = World.GetMutableFrontend().GetSubsystemChecked<Jafg::JMaterialSubsystem>()
        ->GetPbrInstance(JgcInfo,"Materials/Scifi");

    auto& Light{*Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{World})};
    Light.EditorName = "Sun";
    Light.EmplaceRootComponent<Jafg::ALightComponent>([](Jafg::ALightComponent& Comp)
    {
        Comp.SetType(Jafg::ELight::Directional);
        Comp.SetCastShadows(true);
        Comp.SetDirection({-0.5f, -0.5f, 1.0f});
        Comp.SetTint({1.0f, 0.75f, 0.5f});
    });
}

void Jgc::JOwssGenerationSubsystem::GenerateAsteroids(LWorldVec3 const& Where)
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

        Jafg::ARigidComponent& Rigid{Asteroid.EmplaceRootComponent<Jafg::ARigidComponent>()};
        Jafg::AStaticMeshComponent& Mesh{Asteroid.EmplaceSceneComponentTo<Jafg::AStaticMeshComponent>(Rigid, [&](Jafg::AStaticMeshComponent& Comp)
        {
            Comp.SetMesh(JgcInfo, "Models/Asteroids/Object_16.glb", Jafg::EStaticMeshStateBits::PhysicsReady);
            Comp.SetPbrMaterialInstance(this->MaterialInstance);
        })};
        Rigid.SetLocalTransformByTeleport(Trans);
        Rigid.MakeConvex(Mesh.GetMutableMesh());
        Rigid.AddToSimulation(Jafg::ESimulationAddingBehavior::Preserve);
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

Jgc::AVehicleComponent& Jgc::JOwssGenerationSubsystem::SpawnShip(LWorldVec3 const& Where, LWorldQuat const& Orientation /* = maths::identity<LWorldQuat> */)
{
    STAT_FUNCTION()
    check(!!this->MaterialInstance)
    return Jafg::SpawnDeferredObject(Jafg::TWorldStaticInit<Jafg::AActor>{this->GetWorld()})([&](Jafg::AActor& Actor)
    {
        Actor.EditorName = "Ship";
        Actor.SetCanEverTick(true);
        Actor.SetCanEverPhysicsTick(true);
        auto& Rigid{Actor.EmplaceRootComponent<Jafg::ARigidComponent>()};
        Rigid.SetLocalTranslationByTeleport(Where);
        Rigid.SetLocalRotatorByTeleport(Orientation);

        this->GetMutableEngine().GetSubsystemChecked<Jafg::JMeshSubsystem>()->LoadAndProjectScene(
              JgcInfo
            , "Vehicles/SVS26/SVS26.glb"
            , Rigid
            , {.PbrMaterialInstance = this->MaterialInstance}
            , Jafg::EStaticMeshStateBits::PhysicsReady
            );

        /*
         * The view while driving. Behind and slightly above the ship. It hangs on a spring: Accelerations push it
         * away from where it belongs (at most by the maximums), so that speeding up, braking and strafing are felt.
         */
        /* The pivot that free look turns the eye around (see #AJgcVehicleInputComponent). */
        auto& Gimbal{Actor.EmplaceSceneComponentTo<Jafg::AGimbalComponent>(Actor.GetRootComponent())};
        Actor.EmplaceSceneComponentTo<Jafg::AEyeComponent>(Gimbal, [](Jafg::AEyeComponent& Eye)
        {
            Eye.SetLocalTranslationByTeleport(maths::up_vector<LWorldVec3> * 2_r - maths::forward_vector<LWorldVec3> * 30_r);
            Eye.BlendInTime = 0.6f;
            Eye.SpringFrequency = 0.35f;
            Eye.SpringDampingRatio = 0.8f;
            Eye.SpringMaxLateral = 3.0f;
            Eye.SpringMaxVertical = 3.0f;
            Eye.SpringMaxLongitudinal = 6.0f;
            Eye.RotationLagSpeed = 5.0f;
            Eye.MaxRotationLag = 12.0f;
        });

        Actor.EmplaceComponent<AVehicleComponent>([](AVehicleComponent& Comp)
        {
            Comp.SetVehicleType(EVehicle::Dynamic3D);
        });
        /* After the vehicle, so that a driver leaves before the avatar is torn down. */
        Actor.EmplaceComponent<Jafg::AAvatarComponent>();
        Actor.EmplaceComponent<AJgcVehicleInputComponent>();
    })->GetComponentChecked<AVehicleComponent>();
}
