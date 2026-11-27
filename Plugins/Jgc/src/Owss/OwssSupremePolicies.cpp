// Copyright mzoesch. All rights reserved.

#include "Jgc/Owss/OwssSupremePolicies.h"
#include "Framework/AvatarStart.h"
#include "Framework/Editor.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/MeshSubsystem.h"
#include "Framework/MeshSceneProjection.h"
#include "Framework/StaticMeshComponent.h"
#include "Framework/CharacterMovementComponent.h"
#include "Framework/EyeComponent.h"
#include "Framework/LightComponent.h"
#include "User/UserPreferences.h"
#include "Jgc/Framework/CharacterInputComponent.h"
#include "Jgc/Framework/Vehicle.h"
#include "Jgc/Framework/Humanoid.h"
#include "Jgc/Framework/CustomizationStation.h"
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
            Comp.SetMesh(JgcInfo, "Models/Plane_8M.glb");
            Comp.SetPbrMaterialInstance(Frontend.GetSubsystemChecked<Jafg::JMaterialSubsystem>()
                ->GetPbrInstance(JafgInfo,"Textures/Editor/GroundGrid_8M"));
            Comp.SetLocalScaleByTeleport(maths::up_vector<LWorldVec3>
                + (maths::right_vector<LWorldVec3> + maths::forward_vector<LWorldVec3>) * (Scale/4_r));
        });
    }

    auto SpawnObject{[&](LStringView Mesh, TArray<LWorldVec3> const& Translations)
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
                Comp.SetMesh(JgcInfo, Mesh);
                Comp.SetPbrMaterialInstance(Frontend.GetSubsystemChecked<Jafg::JMaterialSubsystem>()
                    ->GetPbrInstance(JafgInfo, "Textures/Editor/ObjectGrid_1M"));
            });
            Rigid.MakeConvex();
            Rigid.AddToSimulation(Jafg::ESimulationAddingBehavior::Preserve);
        }
    }};
    auto SpawnCubes{[&](TArray<LWorldVec3> const& Translations)
    {
        SpawnObject("Models/Cube.glb", Translations);
    }};
    auto SpawnHalfCubes{[&](TArray<LWorldVec3> const& Translations)
    {
        SpawnObject("Models/HalfCube.glb", Translations);
    }};
    auto SpawnQuarterCubes{[&](TArray<LWorldVec3> const& Translations)
    {
        SpawnObject("Models/QuarterCube.glb", Translations);
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

    {
        auto& Statue{*Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{World})};
        Statue.EditorName = "FacelessMale";
        auto& Rigid{Statue.EmplaceRootComponent<Jafg::ARigidComponent>([](Jafg::ARigidComponent& Comp)
        {
            Comp.SetLocalTranslationByTeleport({-3.0, 0.0, 12.5});
            Comp.SetLocalRotatorByTeleport(maths::radians(LWorldVec3{0, 180, 0}));
        })};
        Statue.EmplaceSceneComponentTo<Jafg::AStaticMeshComponent>(Rigid, [&](Jafg::AStaticMeshComponent& Comp)
        {
            auto& Frontend{World.GetMutableFrontend()};
            Comp.SetMesh(JgcInfo, "Rigged/Human/FacelessMale.glb");
            Comp.SetPbrMaterialInstance(Frontend.GetSubsystemChecked<Jafg::JMaterialSubsystem>()
                ->GetPbrInstance(JgcInfo, "Materials/Skin"));
        });
        Rigid.MakeConvex();
        Rigid.AddToSimulation(Jafg::ESimulationAddingBehavior::Preserve);
    }

    /*
     * OpenPBR. A stand per material: In front the plane (tilted towards the viewer), behind it the shader ball, behind
     * and slightly above that Suzanne, and again behind and above the two spheres (smooth and flat shaded). The stands
     * of a section are in rows, the rows behind each other, each with a cool rect light behind and above it. The local
     * +Z of a section points towards its viewer, its local +X to their right.
     * See Content/Materials/OpenPbrExamples (one per feature) and Content/Materials/OpenPbrMaterials (combined ones).
     */
    {
        auto& Materials{*World.GetMutableFrontend().GetSubsystemChecked<Jafg::JMaterialSubsystem>()};

        struct LPart final
        {
            LStringView Name;
            LStringView Mesh;
            LWorldVec3 Offset;
            //# Euler angles in degrees.
            LWorldVec3 Rotation;
            LWorldReal Scale;
            //# Whether only the nodes named "Material..." show the material. The others are neutral.
            bool bMaterialNodesOnly{};
        };
        /*
         * Every part is a scene, as the shader ball is made of many meshes. Only its nodes named "Material..." show
         * the material, the others (its core and background) are neutral, so that the material stands out.
         */
        static constexpr std::array Parts{
            LPart{"Plane", "Models/Plane_1M.glb", {0.0, 0.45, 0.0}, {60.0, 0.0, 0.0}, 1.0},
            LPart{"Shader Ball", "Models/SmoothShaderBall.glb", {0.0, 0.0, -1.4}, {0.0, 0.0, 0.0}, 1.0, true},
            LPart{"Suzanne", "Models/SmoothSuzanne.glb", {0.0, 1.55, -2.9}, {0.0, 0.0, 0.0}, 0.4},
            LPart{"Smooth Sphere", "Models/SmoothUvSphere.glb", {-0.6, 1.75, -4.3}, {0.0, 0.0, 0.0}, 1.0},
            LPart{"Sphere", "Models/UvSphere.glb", {0.6, 1.75, -4.3}, {0.0, 0.0, 0.0}, 1.0},
            };
        constexpr LWorldReal StandWidth{2.6_r};
        constexpr LWorldReal RowDepth{7.0_r};
        Jafg::LPbrMaterialInstanceRef const Neutral{Materials.GetPbrInstance(JgcInfo, "Materials/Neutral")};

        /* Every file is read once. Its meshes are shared by all stands. */
        TArray<Jafg::LMeshScene> Scenes;
        for (auto const& Part: Parts)
        {
            auto Scene{World.GetMutableFrontend().GetEngine().GetSubsystemChecked<Jafg::JMeshSubsystem>()->LoadScene(
                JgcInfo, Part.Mesh, {}, Jafg::EStaticMeshStateBits::PhysicsReady)};
            if (!Scene)
            {
                LOG_FATAL(LogOwss, "[{}]: Failed to load the scene of a part of the OpenPBR stands.", Part.Mesh)
            }
            Scenes.push_back(std::move(*Scene));
        }

        auto SpawnStand{[&](LStringView Material, LWorldVec3 const& Origin, LWorldQuat const& Orientation)
        {
            LStringView const Name{Material.substr(Material.find_last_of('/') + 1)};
            Jafg::LPbrMaterialInstanceRef const Instance{Materials.GetPbrInstance(JgcInfo, Material)};
            for (auto Idx{0uz}; Idx < Parts.size(); ++Idx)
            {
                LPart const& Part{Parts[Idx]};
                auto& Actor{*Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{World})};
                Actor.EditorName = algo::sprintf("{} {}", Name, Part.Name);
                auto& Rigid{Actor.EmplaceRootComponent<Jafg::ARigidComponent>([&](Jafg::ARigidComponent& Comp)
                {
                    Comp.SetLocalTranslationByTeleport(Origin + Orientation * Part.Offset);
                    Comp.SetLocalRotatorByTeleport(Orientation * LWorldQuat{maths::radians(Part.Rotation)});
                })};
                auto const Projection{Jafg::ProjectMeshScene(Scenes[Idx], Rigid, {
                    .PbrMaterialInstance = Instance,
                    .OnNodeProjected = [&Neutral, &Part](Jafg::ASceneComponent& Comp, Jafg::LMeshSceneNode const& Node)
                    {
                        auto* Mesh{Jafg::DynamicCast<Jafg::AStaticMeshComponent>(&Comp)};
                        if (Mesh && Part.bMaterialNodesOnly && !Node.Name.starts_with("Material"))
                        {
                            Mesh->SetPbrMaterialInstance(Neutral);
                        }
                    },
                    })};
                /* The rigid root has to stay uniformly unscaled. */
                for (Jafg::ASceneComponent* Root: Projection.Roots)
                {
                    Root->SetLocalScaleByTeleport(Root->GetLocalScale() * Part.Scale);
                }
                Rigid.MakeConvex();
                Rigid.AddToSimulation(Jafg::ESimulationAddingBehavior::Preserve);
            }
        }};

        auto SpawnSection{[&](LStringView Folder, std::span<LStringView const> Names, std::size_t Columns
            , LWorldVec3 const& Start, LWorldQuat const& Orientation)
        {
            std::size_t const Rows{(Names.size() + Columns - 1) / Columns};
            for (auto Idx{0uz}; Idx < Names.size(); ++Idx)
            {
                LWorldVec3 const Local{StandWidth * static_cast<LWorldReal>(Idx % Columns), 0.0_r, -RowDepth * static_cast<LWorldReal>(Idx / Columns)};
                /* Names with a path are already identifiers. */
                LString const Material{Names[Idx].contains('/') ? LString{Names[Idx]} : algo::sprintf("{}/{}", Folder, Names[Idx])};
                SpawnStand(Material, Start + Orientation * Local, Orientation);
            }

            /* Rim lights. The sun only lights the stands from the front, which hides everything that changes towards grazing angles. */
            for (auto Row{0uz}; Row < Rows; ++Row)
            {
                std::size_t const Count{std::min(Columns, Names.size() - Row * Columns)};
                LWorldReal const Width{StandWidth * static_cast<LWorldReal>(Count)};
                auto& Light{*Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{World})};
                Light.EditorName = algo::sprintf("{} Rim Light {}", Folder.substr(Folder.find_last_of('/') + 1), Row);
                Light.EmplaceRootComponent<Jafg::ALightComponent>([&](Jafg::ALightComponent& Comp)
                {
                    LWorldVec3 const Local{(Width - StandWidth) * 0.5_r, 4.0_r, -RowDepth * static_cast<LWorldReal>(Row) - 5.8_r};
                    Comp.SetType(Jafg::ELight::Rect);
                    Comp.SetLocalTranslationByTeleport(Start + Orientation * Local);
                    Comp.SetDirection(Orientation * maths::normalize(LWorldVec3{0.0_r, -0.5_r, 1.0_r}));
                    Comp.SetSize({Width, 1.0f});
                    Comp.SetTint({0.6f, 0.8f, 1.0f});
                    Comp.SetIntensity(4.0f);
                    Comp.SetRange(std::min(16_r, Width * 0.6_r));
                });
            }
        }};

        /* One stand per feature, on the left of the start. */
        static constexpr std::array<LStringView, 41> Features{
            "BaseWeight", "BaseMetalness", "DiffuseRoughness", "SpecularWeight", "SpecularWeightBoost", "SpecularIor",
            "SpecularColor", "MetalEdgeTint", "Anisotropy", "NormalMap", "Transmission", "TransmissionColor",
            "TransmissionDepth", "TransmissionScatter", "TransmissionScatterAnisotropy", "TransmissionRoughness",
            "Dispersion", "Subsurface", "SubsurfaceScatterAnisotropy", "Coat", "CoatColor", "CoatRoughness",
            "CoatAnisotropy", "CoatIor", "CoatDarkening", "CoatNormal", "Fuzz", "FuzzRoughness", "Emission",
            "EmissionUnderCoat", "ThinFilm", "ThinFilmDielectric", "Opacity", "ThinWalled", "ThinWalledTransmission",
            "AlphaMask", "AlphaBlend", "SpecularTexture", "AnisotropyTexture", "ThinFilmTexture", "SubsurfaceTexture",
            };
        SpawnSection("Materials/OpenPbrExamples", Features, 11, {-14.0, 0.0, 30.0}, LWorldQuat{maths::radians(LWorldVec3{0, 90, 0})});

        /* The materials that the specification mentions, which combine the features. Ahead of the start. */
        static constexpr std::array<LStringView, 36> Combined{
            "AluminumBrushed", "CarPaint", "Glass", "Honey", "Ketchup", "LightBulb", "Pearl", "SoapBubble", "Velvet",
            "Diamond", "Water", "Ice", "WindowGlass", "FrostedGlass", "MilkyGlass", "FruitJuice", "MurkyWater", "Marble",
            "Jade", "Wax", "Porcelain", "Plastic", "Rubber", "Gold", "Copper", "ChippedPaint", "LacqueredWood",
            "CarbonFiber", "Leaf", "Paper", "Lava", "GlowStick", "DustyMetal", "Peach", "DisplayScreen", "Materials/Skin",
            };
        SpawnSection("Materials/OpenPbrMaterials", Combined, 12, {16.0, 0.0, 40.0}, LWorldQuat{maths::radians(LWorldVec3{0, 180, 0})});
    }

    World.GetSubsystemChecked<Jgc::JOwssGenerationSubsystem>()
        ->SpawnShip({10.0f, 3.0f, 15.0f}, LWorldQuat{maths::radians(LWorldVec3{0, -90, 0})});

    Jgc::SpawnCustomizationStation(World, {-3.5, 0.0, 7.5}, LWorldQuat{maths::radians(LWorldVec3{0, 180, 0})});

    // Jgc::SpawnPatrollingHumanoid(World, {{-6.0, 0.1, 6.0}, {6.0, 0.1, 6.0}});
}

void Jgc::AOwssSupremePolicies::BeginClassLife(Jafg::LBeginClassLifeInfo const& Info)
{
    Super::BeginClassLife(Info);

    LOG_TRACE(LogOwss, "Registering owss gym callback.")
    auto& Prefs{Jafg::GetMutableSingleton<Jafg::JUserPreferences>()};
    if (Prefs.EditorObjectCollectionsCallbacks.contains("Gym"))
    {
        LOG_VERBOSE(LogOwss, "Gym callback was registered by another client.")
    }
    else
    {
        Prefs.EditorObjectCollectionsCallbacks["Gym"] = &LoadGymCollection;
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
        bool const bLocal{Pc.IsOwningLackey<Jafg::ELackey::Local>()};
        if (bLocal)
        {
            Result->EmplaceComponent<ACharacterInputComponent>();
        }
        /* Every character has a body. Only local humans can switch to the view from behind. */
        EquipHumanoidBody(*Result, {.bThirdPersonView = bLocal});
    }

    return Result;
}
