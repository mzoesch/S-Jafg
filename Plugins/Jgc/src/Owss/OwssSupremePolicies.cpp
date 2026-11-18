// Copyright mzoesch. All rights reserved.

#include "Jgc/Owss/OwssSupremePolicies.h"
#include "Framework/PawnStart.h"
#include "Framework/Editor.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/StaticMeshComponent.h"
#include "Framework/CharacterMovementComponent.h"
#include "User/UserPreferences.h"
#include "Jgc/Framework/JgcCharacterInputComponent.h"
#include "Jgc/Framework/JgcInputComponent.h"
#include "Jgc/Framework/Vehicle.h"
#include "Jgc/Owss/OwssGenerationSubsystem.h"

static void LoadGymCollection(Jafg::LWorld& World)
{
    Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::APawnStart>{World})
        ->GetRootComponent()
            .SetLocalTranslationByTeleport(maths::up_vector<LWorldVec3> * 1.71_r);

    {
        constexpr auto Scale{100_r};

        auto& Plane{*Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{World})};
        Plane.EditorName = "Ground"sv;
        Plane.SetEditorHitTestable(false);
        auto& Rigid{Plane.EmplaceRootComponent<Jafg::ARigidComponent>()};
        Rigid.SetLocalTranslationByTeleport({0,0,0});
        Rigid.MakeBox(LWorldVec3{Scale,0.0,Scale});
        Rigid.SetShouldRender(true);
        Rigid.AddToSimulation(Jafg::ESimulationAddingBehavior::Preserve);
        Plane.EmplaceSceneComponentTo<Jafg::AStaticMeshComponent>(Rigid, [World=&World](Jafg::AStaticMeshComponent& Comp)
        {
            auto& Frontend{World->GetMutableFrontend()};
            Comp.SetMesh(JafgInfo/LITERAL_TEXT("Models/Plane_8M.glb"));
            Comp.SetPbrMaterialInstance(Frontend.GetSubsystemChecked<Jafg::JMaterialSubsystem>()
                ->GetPbrInstance(JafgInfo,"Textures/Editor/GroundGrid_8M"));
            Comp.SetLocalScaleByTeleport(maths::up_vector<LWorldVec3>
                + (maths::right_vector<LWorldVec3> + maths::forward_vector<LWorldVec3>) * (Scale/4_r));
        });
    }

    {
        auto& Cube{*Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{World})};
        Cube.EditorName = "Cube"sv;
        auto& Rigid{Cube.EmplaceRootComponent<Jafg::ARigidComponent>()};
        Rigid.SetShouldRender(true);
        Rigid.SetLocalTranslationByTeleport({-4.5, 0.5, 12.5});
        Rigid.SetPhysicsMotion(Jafg::EPhysicsMotion::Dynamic);
        Rigid.SetPhysicsLayer(Jafg::EPhysicsLayer::Dynamic);
        Rigid.MakeBox(LWorldVec3{0.5});
        Rigid.AddToSimulation(Jafg::ESimulationAddingBehavior::Activate);
        Rigid.SetLinearVelocity({-5,10,0});
        Cube.EmplaceSceneComponentTo<Jafg::AStaticMeshComponent>(Rigid, [World=&World](Jafg::AStaticMeshComponent& Comp)
        {
            auto& Frontend{World->GetMutableFrontend()};
            Comp.SetMesh(JafgInfo/LITERAL_TEXT("Models/Cube_1M.glb"));
            Comp.SetPbrMaterialInstance(Frontend.GetSubsystemChecked<Jafg::JMaterialSubsystem>()->GetPbrInstance(JafgInfo, "Textures/Editor/ObjectGrid_1M"));
        });
    }

    auto& Generation{*World.GetSubsystemChecked<Jgc::JOwssGenerationSubsystem>()};
    Generation.SpawnShip({10.0f, 3.0f, 15.0f}, LWorldQuat{maths::radians(LWorldVec3{0, -90, 0})});
}

void Jgc::AOwssSupremePolicies::BeginClassLife(Jafg::LBeginClassLifeInfo const& Info)
{
    Super::BeginClassLife(Info);

    LOG_TRACE(LogOwss, "Registering owss gym callback.")
    auto& Prefs{Jafg::GetMutableSingleton<Jafg::JUserPreferences>()};
    if (Prefs.EditorObjectCollectionsCallbacks.contains("JgcGym"))
    {
        LOG_VERBOSE(LogOwss, "JgcGym callback was registered by another client.")
    }
    else
    {
        Prefs.EditorObjectCollectionsCallbacks["JgcGym"] = &LoadGymCollection;
    }
}

void Jgc::AOwssSupremePolicies::EndClassLife(Jafg::LEndClassLifeInfo const& Info)
{
    Super::EndClassLife(Info);
}

void Jgc::AOwssSupremePolicies::OnWorldPostInit()
{
    Super::OnWorldPostInit();
}

void Jgc::AOwssSupremePolicies::OnPersonaControllerCreated(Jafg::APersonaController& Pc)
{
    this->GetWorld().GetSubsystemChecked<JOwssGenerationSubsystem>()->GenerateAsteroids(maths::zero_vector<LWorldVec3>);
    Super::OnPersonaControllerCreated(Pc);
}

TJxxUnique<Jafg::APawn> Jgc::AOwssSupremePolicies::GetPawnForPersonaController(Jafg::APersonaController const& Pc)
{
    auto Result{Super::GetPawnForPersonaController(Pc)};

    if (Result
#if JAFG_WITH_EDITOR
        /* Editor pawns fly freely with their editor camera. */
        && !Pc.GetComponent<Jafg::AEditorPersonaControllerComponent>()
#endif /* JAFG_WITH_EDITOR */
        )
    {
        Result->EmplaceComponent<Jafg::ACharacterMovementComponent>();
        if (Pc.IsOwningLackey<Jafg::ELackey::Local>())
        {
            Result->EmplaceComponent<AJgcCharacterInputComponent>();
            Result->EmplaceComponent<AJgcInputComponent>();
        }
    }

    return Result;
}
