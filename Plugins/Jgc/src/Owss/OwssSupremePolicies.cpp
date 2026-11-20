// Copyright mzoesch. All rights reserved.

#include "Jgc/Owss/OwssSupremePolicies.h"
#include "Framework/AvatarStart.h"
#include "Framework/Editor.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/StaticMeshComponent.h"
#include "Framework/CharacterMovementComponent.h"
#include "Framework/EyeComponent.h"
#include "User/UserPreferences.h"
#include "Jgc/Framework/JgcCharacterInputComponent.h"
#include "Jgc/Framework/Vehicle.h"
#include "Jgc/Owss/OwssGenerationSubsystem.h"

static void LoadGymCollection(Jafg::LWorld& World)
{
    Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AAvatarStart>{World})
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
            Comp.SetMesh(JgcInfo/LITERAL_TEXT("Models/Plane_8M.glb"));
            Comp.SetPbrMaterialInstance(Frontend.GetSubsystemChecked<Jafg::JMaterialSubsystem>()
                ->GetPbrInstance(JafgInfo,"Textures/Editor/GroundGrid_8M"));
            Comp.SetLocalScaleByTeleport(maths::up_vector<LWorldVec3>
                + (maths::right_vector<LWorldVec3> + maths::forward_vector<LWorldVec3>) * (Scale/4_r));
        });
    }

    auto SpawnObject{[&](LPath const& Mesh, TArray<LWorldVec3> const& Translations)
    {
        auto Idx{0uz};
        for (auto& Translation: Translations)
        {
            auto& Cube{*Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{World})};
            Cube.EditorName = algo::sprintf("Cube {}", Idx++);
            auto& Rigid{Cube.EmplaceRootComponent<Jafg::ARigidComponent>([Translation](Jafg::ARigidComponent& Comp)
            {
                Comp.SetLocalTranslationByTeleport(Translation);
            })};
            Cube.EmplaceSceneComponentTo<Jafg::AStaticMeshComponent>(Rigid, [&](Jafg::AStaticMeshComponent& Comp)
            {
                auto& Frontend{World.GetMutableFrontend()};
                Comp.SetMesh(Mesh);
                Comp.SetPbrMaterialInstance(Frontend.GetSubsystemChecked<Jafg::JMaterialSubsystem>()
                    ->GetPbrInstance(JafgInfo, "Textures/Editor/ObjectGrid_1M"));
            });
            Rigid.MakeConvex();
            Rigid.AddToSimulation(Jafg::ESimulationAddingBehavior::Preserve);
        }
    }};
    auto SpawnCubes{[&](TArray<LWorldVec3> const& Translations)
    {
        SpawnObject(JgcInfo/LITERAL_TEXT("Models/Cube.glb"), Translations);
    }};
    auto SpawnHalfCubes{[&](TArray<LWorldVec3> const& Translations)
    {
        SpawnObject(JgcInfo/LITERAL_TEXT("Models/HalfCube.glb"), Translations);
    }};
    auto SpawnQuarterCubes{[&](TArray<LWorldVec3> const& Translations)
    {
        SpawnObject(JgcInfo/LITERAL_TEXT("Models/QuarterCube.glb"), Translations);
    }};

    SpawnCubes({
        {-4.5, 0.0, 12.5},
        {-4.5, 1.0, 12.5},
        {-5.5, 0.0, 12.5},
        {-6.5, 0.0, 12.5},
        });
    SpawnHalfCubes({
        {-7.5, 0.0, 12.5},
        });
    SpawnQuarterCubes({
        {-8.5, 0.0, 12.5},
        });


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

TJxxUnique<Jafg::AActor> Jgc::AOwssSupremePolicies::SpawnAvatarForPersonaController(Jafg::APersonaController const& Pc)
{
    auto Result{Super::SpawnAvatarForPersonaController(Pc)};

    if (Result
#if JAFG_WITH_EDITOR
        /* Editor cameras fly freely. */
        && !Pc.GetComponent<Jafg::AEditorPersonaControllerComponent>()
#endif /* JAFG_WITH_EDITOR */
        )
    {
        Result->EmplaceComponent<Jafg::ACharacterMovementComponent>();
        /* Coming back from another eye (e.g., leaving a vehicle) is blended. */
        if (auto* Head{Jafg::AEyeComponent::FindFirstEyeIn(Result->GetRootComponent())})
        {
            Head->BlendInTime = 0.5f;
        }
        if (Pc.IsOwningLackey<Jafg::ELackey::Local>())
        {
            Result->EmplaceComponent<AJgcCharacterInputComponent>();
        }
    }

    return Result;
}
