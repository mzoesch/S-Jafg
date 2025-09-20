// Copyright mzoesch. All rights reserved.

#include "User/Frontend/Osd/DebugScreen.h"
#include "Build/EngineBuildInfo.h"
#include "Core/Application.h"
#include "Engine/Engine.h"
#include "Framework/Pawn.h"
#include "Framework/PersonaController.h"
#include "MyWorld/ChunkKey.h"
#include "MyWorld/VoxelKey.h"
#include "MyWorld/Chunk/Chunk.h"
#include "System/MaterialSubsystem.h"
#include "User/LocalEgo.h"
#include "User/UserPreferences.h"
#include "Widgets/Spacer.h"
#include "Widgets/TextBox.h"
#include "Widgets/VRegion.h"
#include "Debug/DebugTraceSphere.h"
#include "Debug/DebugTraceCube.h"
#include "Debug/DebugTraceLine.h"
#include "Debug/DebugTracePlane.h"
#include "MyWorld/TimeWorldSubsystem.h"
#include "MyWorld/Validation/ChunkValidationUtility.h"

Jafg::WDebugScreen::WDebugScreen(const LObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->SetShouldTick(true);
    this->SetVisibility(EWidgetVisibility::TransitiveHitTestInvisible);
    return;
}

void Jafg::WDebugScreen::BeginLifeDefault()
{
    Super::BeginLifeDefault();

    if (GEngine == nullptr)
    {
        Tasks::Make(ENamedThreads::Master, ETaskTime::AfterEngineInit, {this, &WDebugScreen::RegisterCliObjects});
    }
    else
    {
        this->RegisterCliObjects();
    }

    return;
}

void Jafg::WDebugScreen::Construct()
{
    Super::Construct();

    const JMaterialSubsystem* MaterialSubsystem = this->GetEngine()->GetSubsystem<JMaterialSubsystem>();

    constexpr f32 SpacerHeight { 20.0f };

    LTextBoxBrush Brush { LTextBoxBrush::Body() };
    Brush.Type = ERegionBrush::Box;
    Brush.Tint = { 0, 0, 0, 128 };

    MakeRootNode(WRegion)
        .Anchor(EAnchor::Fill)
    [
        NewNode(WVRegion)
            .Anchor(EAnchor::TopLeft)
        [

            NewNode(WTextBox)
                .Brush(Brush)
                .Content(LString::SprintF(
                    "JAFG v{} at [{} {}] on [{} - {}] @mzoesch",
                    BuildInfo::GetEngineVersionStr(),
                    BuildInfo::GetBuildDate(),
                    BuildInfo::GetBuildTime(),
                    BuildInfo::GetVcsBranch(),
                    BuildInfo::GetVcsRevision()
                    ))
            +
            NewNode(WTextBox).SaveTo(&this->FpsSection)
                .Brush(Brush)
            +
            NewNode(WTextBox).SaveTo(&this->TimeStatsSection)
                .Brush(Brush)
            +

            NewNode(WSpacer).Height(SpacerHeight)

            +
            NewNode(WTextBox).SaveTo(&this->LocalPawnLocationSection)
                .Brush(Brush)
            +
            NewNode(WTextBox).SaveTo(&this->LocalPawnFacingSection)
                .Brush(Brush)
            +
            NewNode(WTextBox).SaveTo(&this->LocalPawnChunkSection)
                .Brush(Brush)
            +
            NewNode(WTextBox).SaveTo(&this->LocalPawnVoxelSection)
                .Brush(Brush)
            +

            NewNode(WSpacer).Height(SpacerHeight)

            +
            NewNode(WTextBox).SaveTo(&this->MyWorldTimeSection)
                .Brush(Brush)
            +

            NewNode(WSpacer).Height(SpacerHeight)

            +
            NewNode(WRegion)
                .Padding(LPadding(70.0f))
                .Type(ERegionBrush::Box)
                .Texture(&MaterialSubsystem->GetBlendOpaqueAtlasTexture())
            +
            NewNode(WRegion)
                .Padding(LPadding(70.0f))
                .Type(ERegionBrush::Box)
                .Texture(&MaterialSubsystem->GetBlendersAtlasTexture())

        ]
        +
        NewNode(WVRegion)
            .Anchor(EAnchor::TopRight)
        [

            NewNode(WTextBox)
                .Anchor(EAnchor::TopRight)
                .Content("Memory statistics")
                .Brush(Brush)
            +
            NewNode(WTextBox)
                .Anchor(EAnchor::TopRight)
                .Content("Central processing unit information")
                .Brush(Brush)
            +
            NewNode(WTextBox)
                .Anchor(EAnchor::TopRight)
                .Content("Graphics processing unit information")
                .Brush(Brush)
            +
            NewNode(WTextBox)
                .Anchor(EAnchor::TopRight)
                .Content("Display information")
                .Brush(Brush)
            +

            NewNode(WSpacer).Height(SpacerHeight)

            +
            NewNode(WTextBox).SaveTo(&this->LocalPawnTargetVoxelSectionDestroy)
                .Anchor(EAnchor::TopRight)
                .Brush(Brush)
            +
            NewNode(WTextBox).SaveTo(&this->LocalPawnTargetVoxelSectionCreate)
                .Anchor(EAnchor::TopRight)
                .Brush(Brush)

        ]
    ]
    FinishWidgetStyling()

    return;
}

void Jafg::WDebugScreen::Tick()
{
    Super::Tick();

    check( GEngine )

    check( this->LocalPawnLocationSection )
    check( this->LocalPawnFacingSection )
    check( this->LocalPawnChunkSection )
    check( this->LocalPawnVoxelSection )
    check( this->LocalPawnTargetVoxelSectionDestroy )
    check( this->LocalPawnTargetVoxelSectionCreate )

    const LLocalEgo* LocalEgo { GEngine->GetLocalEgo() };
    const APersonaController* Controller { LocalEgo->GetCheckedPossessed() };

    if (Controller->DoesPossess())
    {
        {
            const LVector Location { Controller->GetPossessed()->GetTranslation() };
            this->LocalPawnLocationSection->SetContent(LString::SprintF
            (
                "XYZ: {:.3f} / {:.3f} / {:.3f}",
                Location.X,
                Location.Y,
                Location.Z
            ));
        }

        {
            const LRotator Rotator { Controller->GetPossessed()->GetRotator() };
            LString YawAsText { "N/A" };
            if (Rotator.Yaw >= -45.f && Rotator.Yaw <= 45.f)
            {
                YawAsText = "+X (North)";
            }
            else if (Rotator.Yaw > 45.f && Rotator.Yaw < 135.f)
            {
                YawAsText = "+Y (East)";
            }
            else if (Rotator.Yaw >= 135.f || Rotator.Yaw <= -135.f)
            {
                YawAsText = "-X (South)";
            }
            else if (Rotator.Yaw > -135.f && Rotator.Yaw < -45.f)
            {
                YawAsText = "-Y (West)";
            }
            this->LocalPawnFacingSection->SetContent(LString::SprintF
            (
                "Facing: {} ({:.2f}Y / {:.2f}P)",
                YawAsText, Rotator.Yaw, Rotator.Pitch
            ));
        }

        {
            const LVector Location { Controller->GetPossessed()->GetTranslation() };
            const LChunkKey Key { LChunkKey(Location) };
            this->LocalPawnChunkSection->SetContent(LString::SprintF
            (
                "Chunk: {} {} {}",
                Key.X, Key.Y, Key.Z
            ));
        }

        {
            const LVector Location { Controller->GetPossessed()->GetTranslation() };
            const LVoxelKey Key { LVoxelKey::FromWorldSpace(Location) };
            this->LocalPawnVoxelSection->SetContent(LString::SprintF
            (
                "Local voxel: {} {} {}",
                Key.X, Key.Y, Key.Z
            ));
        }

        {
            this->LocalPawnTargetVoxelSectionDestroy->EmptyContent();
            for (const LHitResult& Hit : Controller->GetPossessed()->GetCurrentGenericTraceResults())
            {
                if (const AChunk* HitChunk = Hit.Actor->As<AChunk>(); HitChunk)
                {
                    const LVoxelKey Key = LVoxelKey::FromWorldSpace(Hit.GlobalWorldLocation);
                    this->LocalPawnTargetVoxelSectionDestroy->SetContent(LString::SprintF("TvD: {} {} {}", Key.X, Key.Y, Key.Z));
                    break;
                }
            }
            if (this->LocalPawnTargetVoxelSectionDestroy->GetContent().IsEmpty())
            {
                this->LocalPawnTargetVoxelSectionDestroy->SetContent("TvD: N/A");
            }
        }

        {
            this->LocalPawnTargetVoxelSectionCreate->EmptyContent();
            for (const LHitResult& Hit : Controller->GetPossessed()->GetCurrentGenericTraceResults())
            {
                if (const AChunk* HitChunk { Hit.Actor->As<AChunk>() }; HitChunk && Hit.SurfaceNormal.IsValid())
                {
                    const LVoxelKey Key { HitChunk->CreateRelativeVoxelKey(Hit.GlobalWorldLocation + Hit.SurfaceNormal.GetValue() * 0.5f) };
                    this->LocalPawnTargetVoxelSectionCreate->SetContent(LString::SprintF("TvC: {} {} {}", Key.X, Key.Y, Key.Z));
                    break;
                }
            }
            if (this->LocalPawnTargetVoxelSectionCreate->GetContent().IsEmpty())
            {
                this->LocalPawnTargetVoxelSectionCreate->SetContent("TvC: N/A");
            }
        }

        for (const LHitResult& Hit : Controller->GetPossessed()->GetCurrentGenericTraceResults())
        {
            const AChunk* HitChunk { Hit.Actor->As<AChunk>() };
            if (HitChunk == nullptr)
            {
                continue;
            }

            LWorld* World { Controller->GetPossessed()->GetWorld() };
            const LChunkKey CKey { HitChunk->GetChunkKey() };

            World->AddTemporalObject(LDebugTraceSphere
            {
                LTemporalWorldObject::DrawOnce, Hit.GlobalWorldLocation, 0.1f,
                LDebugTraceSphereVisualParams{16, 16, Lal::LColor::Green}
            });

            if (Hit.SurfaceNormal.IsValid())
            {
                const LVector WorldHit_Create { Hit.GlobalWorldLocation + Hit.SurfaceNormal.GetValue() * 0.5f };
                const LVoxelKey VKey_Create { LVoxelKey::FromWorldSpace(WorldHit_Create) };
                const LVector WorldSpaceCenter_Create
                {
                    CKey.ToWorldSpace() + LVector
                    {
                        static_cast<f32>(VKey_Create.X),
                        static_cast<f32>(VKey_Create.Y),
                        static_cast<f32>(VKey_Create.Z)
                    }
                };
                World->AddTemporalObject(LDebugTraceLine
                {
                    LTemporalWorldObject::DrawOnce, Hit.GlobalWorldLocation, Hit.GlobalWorldLocation + Hit.SurfaceNormal.GetValue(),
                    LDebugTraceLineVisualParams{Lal::LColor::Magenta}
                });
                World->AddTemporalObject(LDebugTraceSphere
                {
                    LTemporalWorldObject::DrawOnce, WorldHit_Create, 0.1f,
                    LDebugTraceSphereVisualParams{16, 16, Lal::LColor::Magenta}
                });
                World->AddTemporalObject(LDebugTraceSphere
                {
                    LTemporalWorldObject::DrawOnce,
                    CKey.ToWorldSpace() + HitChunk->CreateRelativeVoxelKey(WorldHit_Create).ToWorldSpace() + LVector(0.5f), 0.6f,
                    LDebugTraceSphereVisualParams{16, 16, Lal::LColor::Green}
                });
                World->AddTemporalObject(LDebugTraceCube
                {
                    LTemporalWorldObject::DrawOnce, WorldSpaceCenter_Create, LVector::OneVector,
                    LDebugTraceCubeVisualParams{Lal::LColor::Blue}
                });
            }
            break;
        }

        /* Chunk debug lines. */
        if
        (
            const LCliVariable* Var { GEngine->GetCommandLineInterface()->GetVariable("ShowChunkBordersInDebugScreen") };
            Var && Var->GetValue<bool>()
        )
        {
            LWorld* World { Controller->GetPossessed()->GetWorld() };

            const LVector PawnTranslation { Controller->GetPossessed()->GetTranslation() };
            const LChunkKey CKey { PawnTranslation };
            const LVector ChunkCenter { CKey.ToWorldSpace() };

            World->AddTemporalObject(LDebugTraceCube
            {
                LTemporalWorldObject::DrawOnce, LVector::ZeroVector + ChunkCenter, LVector::OneVector * MwStatics::ChunkSize,
                LDebugTraceCubeVisualParams{Lal::LColor::Yellow}
            });

            for (i32 izDelta { 2 }; izDelta < MwStatics::ChunkSize; izDelta += 2)
            {
                const float zDelta = static_cast<float>(izDelta);
                const LVector P1 { ChunkCenter + LVector::UpVector * zDelta };
                const LVector P2 { P1 + LVector::RightVector * MwStatics::ChunkSize };
                const LVector P3 { P1 + (LVector::RightVector + LVector::ForwardVector) * MwStatics::ChunkSize };
                const LVector P4 { P1 + LVector::ForwardVector * MwStatics::ChunkSize };

                World->AddTemporalObject(LDebugTracePlane
                {
                    LTemporalWorldObject::DrawOnce,
                    P1, P2, P3, P4,
                    LDebugTracePlaneVisualParams{Lal::LColor::Yellow}
                });
            }

            for (const LChunkKey& Key : CKey.GetNeighboringChunkKeys())
            {
                World->AddTemporalObject(LDebugTraceCube
                {
                    LTemporalWorldObject::DrawOnce, LVector::ZeroVector + Key.ToWorldSpace(), LVector::OneVector * MwStatics::ChunkSize,
                    LDebugTraceCubeVisualParams{Lal::LColor::Red}
                });
            }

            for
            (
                const TArray<LChunkKey> OtherChunks { Validation::GetAllChunksFromCenterAsBox(CKey, 5, 3, CKey.Z - 1) };
                const LChunkKey& Key : OtherChunks
            )
            {
                if
                (
                       (Key.X == CKey.X + 0 && Key.Y == CKey.Y + 0)
                    || (Key.X == CKey.X + 1 && Key.Y == CKey.Y + 0)
                    || (Key.X == CKey.X + 0 && Key.Y == CKey.Y + 1)
                    || (Key.X == CKey.X + 1 && Key.Y == CKey.Y + 1)
                )
                {
                    continue;
                }

                World->AddTemporalObject(LDebugTraceLine
                {
                    LTemporalWorldObject::DrawOnce,
                    Key.ToWorldSpace() + LVector::DownVector * MwStatics::ChunkSize * 10,
                    Key.ToWorldSpace() + LVector::UpVector * MwStatics::ChunkSize * 10,
                    LDebugTraceLineVisualParams{Lal::LColor::Blue}
                });

                continue;
            }
        }
    }
    else
    {
        this->LocalPawnLocationSection->SetContent("XYZ: [ERR: No pawn]");
        this->LocalPawnFacingSection->SetContent("Facing: [ERR: No pawn]");
        this->LocalPawnChunkSection->SetContent("Chunk: [ERR: No pawn]");
        this->LocalPawnVoxelSection->SetContent("Local voxel: [ERR: No pawn]");
        this->LocalPawnTargetVoxelSectionDestroy->SetContent("TvD: [ERR: No pawn]");
        this->LocalPawnTargetVoxelSectionCreate->SetContent("TvC: [ERR: No pawn]");
    }

    this->InvalidateCacheTime -= Application::GetDeltaTime();
    if (this->InvalidateCacheTime <= 0.0)
    {
        this->InvalidateCacheTime = this->ResetTime;
        this->SlowTick();
    }

    return;
}

void Jafg::WDebugScreen::OnGarbageDefault()
{
    Super::OnGarbageDefault();

    if (GEngine)
    {
        this->UnregisterCliObjects();
    }

    return;
}

void Jafg::WDebugScreen::SlowTick()
{
    check( this->FpsSection )
    check( this->TimeStatsSection )
    check( this->MyWorldTimeSection )

    const JUserPreferences* UserPreferences { GetDefault<JUserPreferences>() };

    this->FpsSection->SetContent(LString::SprintF
    (
        "{} Fps @ {:.2f} ms T: {}; VSync: {} - Fcsssi: {}; L@{:.2f} Lh@{:.2f} I@{:.2f}",
        static_cast<i32>(Application::GetCurrentFps()),
        Application::GetDeltaTimeAsFloat() * 1'000.0f,
        UserPreferences->MaxFps,
        UserPreferences->bVSyncEnabled,
        Application::GetFrameCount(),
        Application::GetLostDeltaTime() * 1'000.0f,
        Application::GetHighestLostDeltaTime() * 1'000.0f,
        Application::GetIdleDeltaTime() * 1'000.0f
    ));

    this->TimeStatsSection->SetContent(LString::SprintF
    (
        "Steady: Avg {}fps/{:.2f}ms; Low: {}fps/{:.2f}ms; High: {}fps/{:.2f}ms",
        static_cast<i32>(static_cast<double>(Application::GetPreviousFrameCount()) / Application::GetRealTimeOfPreviousStatisticsDuration()),
        static_cast<float>(1.0 / (static_cast<double>(Application::GetPreviousFrameCount()) / Application::GetRealTimeOfPreviousStatisticsDuration())) * 1'000.0f,
        static_cast<i32>(1.0 / Application::GetPreviousHighestDeltaTime()),
        Application::GetPreviousHighestDeltaTime() * 1'000.0f,
        static_cast<i32>(1.0 / Application::GetPreviousLowestDeltaTime()),
        Application::GetPreviousLowestDeltaTime() * 1'000.0f
    ));

    if (this->GetOuter()->IsWorld())
    {
        const LWorld* World { static_cast<LWorld*>(this->GetOuter()) };

        if (const JTimeWorldSubsystem* TimeSubsystem { World->GetSubsystem<JTimeWorldSubsystem>() }; TimeSubsystem)
        {
            this->MyWorldTimeSection->SetContent(LString::SprintF
            (
                "T: {} [{} {}] {} [{:.3f}% {}-{}]",
                TimeSubsystem->IsDay() ? 'D' : 'N',
                TimeSubsystem->GetInterpolatedTimeAsItWouldBeOnEarth(JTimeWorldSubsystem::HHMM),
                TimeSubsystem->GetDayCycleAsItWouldBeOnEarth(JTimeWorldSubsystem::DDMMYYYY),
                TimeSubsystem->GetDayTime(),
                TimeSubsystem->GetDayTimeInPercentage(),
                TimeSubsystem->GetMinDayTime(),
                TimeSubsystem->GetMaxDayTime()
            ));
        }
        else
        {
            this->MyWorldTimeSection->SetContent("MyWorld Time: [ERR: No time subsystem]");
        }
    }

    return;
}

void Jafg::WDebugScreen::RegisterCliObjects()
{
    check( GEngine )
    check( this->IsDefault() )

    this->VariableHandle_ShowChunkBordersInDebugScreen =
        GEngine->GetCommandLineInterface()->RegisterVariable({"ShowChunkBordersInDebugScreen", LCliType::Type<bool>()});

    return;
}

void Jafg::WDebugScreen::UnregisterCliObjects()
{
    check( GEngine )
    check( this->IsDefault() )

    if (ensure(this->VariableHandle_ShowChunkBordersInDebugScreen.IsValid()))
    {
        GEngine->GetCommandLineInterface()->UnregisterCommand(&this->VariableHandle_ShowChunkBordersInDebugScreen);
    }

    return;
}
