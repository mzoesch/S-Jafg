// Copyright mzoesch. All rights reserved.

#include "Jgc/Framework/JgcHumanoid.h"
#include "Framework/Actor.h"
#include "Framework/EyeComponent.h"
#include "Framework/MeshSubsystem.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/SkeletalMeshComponent.h"
#include "Framework/CharacterMovementComponent.h"
#include "Framework/Frontend.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Jgc/Framework/JgcCharacterAnimationComponent.h"
#include "Jgc/Framework/JgcCameraBoomComponent.h"
#include "Jgc/Framework/JgcPatrolComponent.h"

Jafg::ASkeletalMeshComponent* Jgc::EquipHumanoidBody(Jafg::AActor& Character, LHumanoidBodyInfo const& Info /* = {} */)
{
    check(Character.HasRootComponent())
    auto& Root{Character.GetRootComponent()};

    Jafg::LSkeletalMeshRef Mesh{Character.GetEngine().GetSubsystemChecked<Jafg::JMeshSubsystem>()->LoadSkeletalMesh(JgcInfo, Info.Mesh)};
    if (!Mesh)
    {
        LOG_ERROR(LogJgc, "[{}]: Failed to load the body [{}]. The character has none.", Character.GetNameAsString(), Info.Mesh)
        return nullptr;
    }

    /* Scene components only draw their children if they draw themselves. A plain root draws nothing but them. */
    Root.SetShouldRender(true);

    auto& Body{Character.EmplaceSceneComponentTo<Jafg::ASkeletalMeshComponent>(Root, [&](Jafg::ASkeletalMeshComponent& Comp)
    {
        Comp.SetMesh(std::move(Mesh));
        for (LStringView const Animations: Info.Animations)
        {
            Comp.AddAnimations(JgcInfo, Animations);
        }
        Comp.SetPbrMaterialInstance(Character.GetWorld().GetMutableFrontend().GetSubsystemChecked<Jafg::JMaterialSubsystem>()
            ->GetPbrInstance(JgcInfo, Info.PbrMaterial));
        Comp.FirstPersonBody = Jafg::EFirstPersonBody::Partial;
        Comp.SetFirstPersonJoints(Info.FirstPersonJoints);
    })};

    Character.EmplaceComponent<AJgcCharacterAnimationComponent>();

    /* The movement drives the first eye directly below the root (see #ACharacterMovementComponent::GetHeadEye). */
    Jafg::AEyeComponent* Head{};
    for (auto const& Child: Root.GetChildren())
    {
        if ((Head = Child->As<Jafg::AEyeComponent>()))
        {
            break;
        }
    }
    if (!Head)
    {
        LOG_WARNING(LogJgc, "[{}]: The character has no head eye. Its body is drawn from every eye.", Character.GetNameAsString())
        return &Body;
    }
    Head->bFirstPerson = true;

    if (Info.bThirdPersonView)
    {
        auto& Boom{Character.EmplaceSceneComponentTo<AJgcCameraBoomComponent>(*Head)};
        Character.EmplaceSceneComponentTo<Jafg::AEyeComponent>(Boom, [Head](Jafg::AEyeComponent& Eye)
        {
            Eye.VertFov = Head->VertFov;
            Eye.NearFrustum = Head->NearFrustum;
            Eye.FarFrustum = Head->FarFrustum;
            Eye.BlendInTime = 0.35f;
        });
    }

    return &Body;
}

Jafg::AActor& Jgc::SpawnPatrollingHumanoid(Jafg::LWorld& World, TArray<LWorldVec3> Waypoints, LHumanoidBodyInfo const& Info /* = {} */)
{
    check(!Waypoints.empty())

    return *Jafg::SpawnDeferredObject(Jafg::TWorldStaticInit<Jafg::AActor>{World})([&](Jafg::AActor& Actor)
    {
        Actor.EditorName = "Patrolling humanoid";
        Actor.SetCanEverTick(true);
        Actor.SetCanEverPhysicsTick(true);

        auto& Root{Actor.EmplaceRootComponent<Jafg::ASceneComponent>()};
        Root.SetLocalTranslationByTeleport(Waypoints.front());
        Actor.EmplaceSceneComponentTo<Jafg::AEyeComponent>(Root);
        Actor.EmplaceComponent<Jafg::ACharacterMovementComponent>();

        EquipHumanoidBody(Actor, Info);

        Actor.EmplaceComponent<AJgcPatrolComponent>([&](AJgcPatrolComponent& Comp)
        {
            Comp.Waypoints = std::move(Waypoints);
        });
    });
}
